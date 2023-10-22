#include <time.h>
#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "estimate.h"
#include "string.h"
#include "./views_funcs.h"

// safe string functions
int sstrlen(char *s) {
  if(s == NULL || *s == 0) return 0;
  return strlen(s);
}

void HashSet(Hash **r, char *key, void *value) {
  Hash *h = malloc(sizeof(Hash));
  h->key = key;
  h->value = value;
  h->next = *r;
  *r = h;
}

void *HashGet(Hash *r, void *key) {
  for (Hash *c = r; c != NULL; c = c->next) {
    if (strcmp(c->key, key) == 0)
      return c->value;
  }

  return NULL;
}

void HashPrint(Hash *r) {
  for (Hash *c = r; c != NULL; c = c->next)
    printf("%s -> %s\n", c->key, (char *) c->value);
}

void HashFree(Hash *r) {
  for (Hash *c = r; c != NULL;) {
    Hash *n = c->next;
    free(c);
    c = n;
  }
}

Hash *boards = NULL;

void BoardVotesFree(Board *b) {
  for(Vote *v = b->votes; v != NULL;) {
    Vote *n = v->next;
    free(v->vote);
    free(v->user);
    free(v);
    v = n;
  }
  b->votes_count = 0;
}

void BoardTouch(Board *b) { b->updated_at = time(NULL); }

bool BoardUserVoted(Board *board, UUID userid) {
  for (Vote *v = board->votes; v != NULL; v = v->next)
    if (strcmp(v->user, userid) == 0)
      return true;

  return false;
}

void CharConcatAndFree(char **dest, ...) {
  va_list ptr;
  va_start(ptr, dest);

  char *s;

  while((s = va_arg(ptr, char *)) != NULL) {
    int destlen = sstrlen(*dest);
    char *newdest = (char *)calloc(1, destlen + sstrlen(s) + 1);
    if (destlen > 0) {
      strcpy(newdest, *dest);
      strcat(newdest, s);
      free(*dest);
    } else {
      strcat(newdest, s);
    }
    free(s);
    *dest = newdest;
  }

  va_end(ptr);
}

void MemoryTrack(Memory **r, void *v) {
  Memory *m = malloc(sizeof(Memory));
  m->memory = v;
  m->next = *r;
  *r = m;
}

void MemoryFree(Memory *r) {
  for (Memory *c = r; c != NULL;) {
    Memory *n = c->next;
    free(c->memory);
    free(c);
    c = n;
  }
}

void RequestFree(Request *r) {
  HashFree(r->params);
  HashFree(r->body);
  HashFree(r->cookie);
  MemoryFree(r->memory);
  free(r);
}

void ResponseFree(Response *w) {
  HashFree(w->headers);
  HashFree(w->cookie);
  MemoryFree(w->memory);
  free(w);
}

char *CookieSerialize(char *key, char *value) {
  char *escaped_value = curl_easy_escape(NULL, value, 0);

  int size = sstrlen(key)+1+sstrlen(escaped_value)+1; // key + = + value + \0
  char *s = malloc(size);
  sprintf(s, "%s=%s", key, escaped_value);

  curl_free(escaped_value);

  return s;
}

char *CookieUnserialize(const char *value) {
  char *unescaped_value = curl_easy_unescape(NULL, value, 0, NULL);
  char *s = strdup(unescaped_value);

  curl_free(unescaped_value);

  return s;
}

void WriteHeader(Response *w, char *key, char *value) {
  HashSet(&w->headers, key, value);
}

char *FileContent(const char *path) {
  FILE *f = fopen(path, "r");
  if(f == NULL){
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  char *content = malloc(size+1);

  fseek(f, 0, SEEK_SET);
  fread(content, 1, size, f);
  content[size] = 0;
  fclose(f);

  return content;
}

Method String2Method(const char *method) {
  if (strcmp(method, "GET") == 0)
    return METHOD_GET;

  if (strcmp(method, "POST") == 0)
    return METHOD_POST;

  return METHOD_INVALID;
}

bool PathIs(const Request *r, const char *path) {
  return strcmp(r->path, path) == 0;
}

void Redirect(Response *w, char *path) {
  // This list uses SEE OTHER instead of FOUND as POST requests responding with
  // FOUND means the path changes without method changes in most cases we want
  // to change the method to GET. and SEE OTHER does that
  // https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/302
  w->status = 303;
  w->body = "Found";
  WriteHeader(w, "Location", path);
}

UUID EnsureUser(Response *w, const Request *r) {
  char *userid = HashGet(r->cookie, "userid");
  if( userid == NULL ) {
    char *path = strdup(r->path);
    MemoryTrack(&w->memory, path);

    HashSet(&w->cookie, "back", path);
    Redirect(w, "/username");

    return NULL;
  }

  return userid;
}

Board *EnsureBoard(Response *w, const Request *r) {
  UUID boardid = HashGet(r->params, "board");
  if(boardid == NULL){
    Redirect(w, "/");
    return NULL;
  }

  Board *board = HashGet(boards, boardid);
  if(board == NULL){
    Redirect(w, "/");
    return NULL;
  }

  return board;
}

UUID NewUUID() {
  uuid_t uuid;
  uuid_generate(uuid);
  char *str = malloc(UUID_STR_LEN);
  uuid_unparse_lower(uuid, str);
  return str;
}

char *ParamsGet(const Request *r, const char *key) {
  // TODO get query parameter value for key
  return NULL;
}

void BoardFreeOptions(Option *options) {
  for (Option *option = options; option != NULL; ) {
    Option *next = option->next;
    free(option->value);
    free(option);
    option = next;
  }
}

void BoardFree(Board *b) {
  free(b->id);
  free(b->options_str);
  free(b->userid);

  for(Vote *v = b->votes; v!=NULL;){
    Vote *n = v->next;
    free(v->vote);
    free(v->user);
    free(v);
    v = n;
  }

  for(Hash *h=b->votes_stats; h!=NULL;){
    Hash *n = h->next;
    free(n->key);
    h = n;
  }

  HashFree(b->votes_stats);
  BoardFreeOptions(b->options);
}

void BoardSetOptions(Board *board, char *options_str) {
  if (options_str == NULL)
    return;

  board->options_str = strdup(options_str);
  Option *old_options = board->options;
  char *saveptr;
  char *val = strtok_r(options_str, "\n", &saveptr);

  Option *new_options = NULL;
  Option *last_option = NULL;
  while (val != NULL && *val != 0) {
    Option *opt = calloc(1, sizeof(Option));
    opt->value = strdup(val);
    opt->next = NULL;
    if (new_options == NULL) {
      new_options = opt;
    }
    if (last_option != NULL) {
      last_option->next = opt;
    }
    last_option = opt;

    val = strtok_r(NULL, "\n", &saveptr);
  }

  board->options = new_options;
  BoardFreeOptions(old_options);
}

// Handlers
// =========
Board defaults = {
    .id = NULL,
    .userid = NULL,
    .options = NULL,
    .options_str = "1\n2\n3\n5\n8",
    .hidden = true,
    .votes = NULL,
    .votes_count = 0,
    .votes_stats = NULL,
    .updated_at = 0,
};

void RootHandler(Response *w, const Request *r) {
  if (EnsureUser(w, r) == NULL)
    return;

  CharConcatAndFree((char **)&w->body,
                    views_header_html(NULL),
                    views_index_html((void *) &defaults),
                    views_footer_html(NULL),
                    NULL);
  w->freebody = true;
}

void GetUsernameHandler(Response *w, const Request *r) {
  CharConcatAndFree((char **)&w->body,
                    views_header_html(NULL),
                    views_username_html(NULL),
                    views_footer_html(NULL),
                    NULL);
  w->freebody = true;
}

void PostUsernameHandler(Response *w, const Request *r) {
  if (HashGet(r->cookie, "userid") == NULL) {
    HashSet(&w->cookie, "username", HashGet(r->body, "username"));
    UUID userid = NewUUID();
    MemoryTrack(&w->memory, userid);
    HashSet(&w->cookie, "userid", userid);
  }

  char *back = HashGet(r->cookie, "back");
  if ( back == NULL ) return Redirect(w, "/");
  Redirect(w, back);
}

void GetBoardHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if( (board = EnsureBoard(w,r)) == NULL)
    return;

  if(!(BoardUserVoted(board, userid) || strcmp(board->userid, userid) == 0)){}

  BoardPage boardpage = {
    .board = board,
    .user = userid,
  };

  CharConcatAndFree((char **)&w->body,
                    views_header_html(NULL),
                    views_board_html(&boardpage),
                    views_footer_html(NULL),
                    NULL);
  w->freebody = true;
}

void PostBoardsHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board = (Board *)calloc(1, sizeof(Board));
  board->id = NewUUID();
  board->userid = strdup(userid);
  BoardSetOptions(board, HashGet(r->body, "options"));
  board->hidden = true;
  board->updated_at = time(NULL);

  HashSet(&boards, board->id, board);

  char *prefix = "/boards?board=";
  char *path = calloc(1, sstrlen(board->id) + sstrlen(prefix) + 1);
  sprintf(path, "%s%s", prefix, board->id);
  MemoryTrack(&w->memory, path);

  Redirect(w, path);
}

void GetBoardEditHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if ((board = EnsureBoard(w, r)) == NULL)
    return;

  if (strcmp(board->userid, userid) != 0) {
    char *prefix = "/boards?board=";
    char *path = calloc(1, sstrlen(board->id) + sstrlen(prefix) + 1);
    sprintf(path, "%s%s", prefix, board->id);
    MemoryTrack(&w->memory, path);
    Redirect(w, path);
    return;
  }

  CharConcatAndFree((char **)&w->body, views_header_html(NULL),
                    views_index_html(board), views_footer_html(NULL), NULL);
  w->freebody = true;
}

void PostBoardEditHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if ((board = EnsureBoard(w, r)) == NULL)
    return;

  char *prefix = "/boards?board=";
  char *path = calloc(1, sstrlen(board->id) + sstrlen(prefix) + 1);
  sprintf(path, "%s%s", prefix, board->id);
  MemoryTrack(&w->memory, path);

  if (strcmp(board->userid, userid) != 0) {
    Redirect(w, path);
    return;
  }

  BoardSetOptions(board, HashGet(r->body, "options"));
  board->updated_at = time(NULL);

  Redirect(w, path);
}

void GetBoardResetHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if ((board = EnsureBoard(w, r)) == NULL)
    return;

  char *prefix = "/boards?board=";
  char *path = calloc(1, sstrlen(board->id) + sstrlen(prefix) + 1);
  sprintf(path, "%s%s", prefix, board->id);
  MemoryTrack(&w->memory, path);

  if (strcmp(board->userid, userid) != 0) {
    Redirect(w, path);
    return;
  }

  BoardVotesFree(board);
  board->hidden = true;
  BoardTouch(board);

  Redirect(w, path);
}

void GetBoardVoteHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if ((board = EnsureBoard(w, r)) == NULL)
    return;

  CharConcatAndFree((char **)&w->body, views_header_html(NULL),
                    views_vote_html(board), views_footer_html(NULL), NULL);
  w->freebody = true;
}

void PostBoardVoteHandler(Response *w, const Request *r) {
  UUID userid;
  if ((userid = EnsureUser(w, r)) == NULL)
    return;

  Board *board;
  if ((board = EnsureBoard(w, r)) == NULL)
    return;

  char *prefix = "/boards?board=";
  char *path = calloc(1, sstrlen(board->id) + sstrlen(prefix) + 1);
  sprintf(path, "%s%s", prefix, board->id);
  MemoryTrack(&w->memory, path);

  if (BoardUserVoted(board, userid)) {
    Redirect(w, path);
    return;
  }

  HashPrint(r->body);
  char *vote = HashGet(r->body, "vote");
  if (vote == NULL) {
    Redirect(w, path);
    return;
  }

  // find the option and if not found don't proceed
  Option *option;
  for (option = board->options;
       option != NULL && strcmp(vote, option->value) != 0;
       option = option->next);
  if (option == NULL){
    Redirect(w, path);
    return;
  }

  // if vote already exists don't proceed
  Vote *v = calloc(1, sizeof(Vote));
  v->vote = strdup(vote);
  v->user = strdup(userid);
  v->next = board->votes;
  board->votes = v;
  board->votes_count++;

  // update stats
  Hash *stat = HashGet(board->votes_stats, vote);
  if (stat == NULL) {
    HashSet(&board->votes_stats, vote, (void *)1);
    Redirect(w, path);
    return;
  }
  stat->value++;
}

void GetBoardShowHandler(Response *w, const Request *r) {
  // TODO
}

void GetBoardHideHandler(Response *w, const Request *r) {
  // TODO
}

void GetBoardCheckUpdateHandler(Response *w, const Request *r) {
  // TODO
}

// Router
// this is the main router, it inspects the request properties and calls the
// correct handler function
// ==============================================================================
void Router(Response *w, const Request *r) {
  bool is_GET = r->method == METHOD_GET;
  bool is_POST = r->method == METHOD_POST;

  if (is_GET && PathIs(r, "/")) return RootHandler(w, r);

  if (is_GET && PathIs(r, "/username")) return GetUsernameHandler(w, r);
  if (is_POST && PathIs(r, "/username")) return PostUsernameHandler(w, r);

  if (is_GET && PathIs(r, "/boards")) return GetBoardHandler(w, r);
  if (is_POST && PathIs(r, "/boards")) return PostBoardsHandler(w, r);

  if (is_GET && PathIs(r, "/boards/edit")) return GetBoardEditHandler(w, r);
  if (is_POST && PathIs(r, "/boards/edit")) return PostBoardEditHandler(w, r);

  if (is_GET && PathIs(r, "/boards/reset")) return GetBoardResetHandler(w, r);
  if (is_GET && PathIs(r, "/boards/vote")) return GetBoardVoteHandler(w, r);
  if (is_POST && PathIs(r, "/boards/vote")) return PostBoardVoteHandler(w, r);

  if (is_GET && PathIs(r, "/boards/show")) return GetBoardShowHandler(w, r);
  if (is_GET && PathIs(r, "/boards/hide")) return GetBoardHideHandler(w, r);
  if (is_GET && PathIs(r, "/boards/check")) return GetBoardCheckUpdateHandler(w, r);

  w->status = 404;
  w->body = "Page Not Found";
}

void Handler(Response *w, const Request *r) {
  Router(w, r);

  if (w->status == 0) {
    if ( w->body == NULL || *(char *)w->body == 0) {
      w->status = 204;
    }else{
      w->status = 200; // no content
    }
  }

  if ( HashGet(w->headers, "Content-Type") == NULL )
    WriteHeader(w, "Content-Type", "text/html");

  printf("%d %s ... %d\n", r->method, r->path, w->status);
}


// Setup to invoke router
// ==================================================================

enum MHD_Result post_iterator(void *cls, enum MHD_ValueKind kind,
                              const char *key, const char *filename,
                              const char *content_type,
                              const char *transfer_encoding, const char *data,
                              uint64_t off, size_t size) {
  Request *r = cls;

  char *k = strdup(key);
  MemoryTrack(&r->memory, k);

  char *v = malloc(size + 1);
  MemoryTrack(&r->memory, v);
  memcpy(v, data, size);
  v[size] = 0;

  HashSet(&r->body, k, v);

  return MHD_YES;
}

enum MHD_Result value_iterator(void *cls, enum MHD_ValueKind kind,
                                const char *key, const char *value) {

  Request *r = cls;
  char *unserialized_value = CookieUnserialize(value);
  MemoryTrack(&r->memory, unserialized_value);

  char *k = strdup(key);
  MemoryTrack(&r->memory, k);

  if( kind == MHD_COOKIE_KIND ) {
    HashSet(&r->cookie, k, unserialized_value);
  } else if (kind == MHD_GET_ARGUMENT_KIND) {
    HashSet(&r->params, k, unserialized_value);
  }

  return MHD_YES;
}

void request_completed(void *cls, struct MHD_Connection *connection,
                       void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct Request *r = *con_cls;

  if (NULL == r)
    return;

  if (r->method == METHOD_POST) {
    MHD_destroy_post_processor(r->postprocessor);
  }

  RequestFree(r);
  *con_cls = NULL;
}

static enum MHD_Result AccessCallback(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method_str,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {

  Method method = String2Method(method_str);

  // unexpected method
  if (method == METHOD_INVALID)
    return MHD_NO;

  // upload data in a GET!?
  if (method == METHOD_GET && *upload_data_size != 0)
    return MHD_NO;

  Request *r;

  if ( *con_cls != NULL) {
    r = *con_cls;
  } else {
    r = (Request *)calloc(1, sizeof(Request));
    r->method = method;
    r->path = url;

    if (r->method == METHOD_POST) {
      r->postprocessor = MHD_create_post_processor(connection, POSTBUFFERSIZE, &post_iterator, r);
    }

    *con_cls = r;

    return MHD_YES;
  }

  if (r->method == METHOD_POST && *upload_data_size != 0) {
    MHD_post_process(r->postprocessor, upload_data, *upload_data_size);
    *upload_data_size = 0;

    return MHD_YES;
  }

  MHD_get_connection_values(connection, MHD_COOKIE_KIND|MHD_GET_ARGUMENT_KIND, &value_iterator, r);

  Response *w = (Response *)calloc(1, sizeof(Response));

  Handler(w, r);

  enum MHD_ResponseMemoryMode freebody =
      (w->freebody) ? MHD_RESPMEM_MUST_FREE : MHD_RESPMEM_PERSISTENT;

  int size = sstrlen(w->body);

  struct MHD_Response *response =
      MHD_create_response_from_buffer(size, w->body, freebody);

  // Set headers
  for (Hash *h = w->headers; h != NULL; h = h->next)
    MHD_add_response_header(response, h->key, h->value);

  // Set cookie
  for (Hash *h = w->cookie; h != NULL; h = h->next) {
    char *cookie = CookieSerialize(h->key, h->value);
    MemoryTrack(&w->memory, cookie);
    MHD_add_response_header(response, MHD_HTTP_HEADER_SET_COOKIE, cookie);
  }

  enum MHD_Result ret = MHD_queue_response(connection, w->status, response);

  MHD_destroy_response(response);
  ResponseFree(w);

  return ret;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("%s PORT\n", argv[0]);
    return 1;
  }

  CURL *curl = curl_easy_init();

  int port = atoi(argv[1]);

  struct MHD_Daemon *d = MHD_start_daemon(
      MHD_USE_AUTO_INTERNAL_THREAD, port, NULL, NULL, &AccessCallback, NULL,
      MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
      MHD_OPTION_END);
  if (d == NULL) return 1;

  for(Hash *h = boards; h!=NULL; h = h->next) {
    free(h->key);
    Board *b = h->value;
    BoardFree(b);
  }

  HashFree(boards);

  printf("Server started on port %d\n", port);
  getc(stdin);
  MHD_stop_daemon(d);
  curl_easy_cleanup(curl);
  return 0;
}
