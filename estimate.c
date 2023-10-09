#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include <curl/easy.h>


#define POSTBUFFERSIZE 1024

// Data structures
// ===================================================================

typedef enum Method {
  METHOD_INVALID,
  METHOD_GET,
  METHOD_POST
} Method;

typedef int Status;

typedef struct Hash {
  char *key;
  char *value;
  struct Hash *next;
} Hash;

typedef struct Memory {
  void *memory;
  struct Memory *next;
} Memory;

typedef struct Request {
  Method method;
  const char *path;
  Hash *cookie;
  Hash *body;
  // To track heap memory and free it later
  Memory *memory;
  // This section is for MHD variables
  struct MHD_PostProcessor *postprocessor;
} Request;

typedef struct Response {
  Status status;
  void *body;
  bool freebody;
  Hash *headers;
  Hash *cookie;
  // To track heap memory and free it later
  Memory *memory;
} Response;

typedef char *UserID;

typedef struct Board {

} Board;

// Helpers
// ========

void HashSet(Hash **r, char *key, char *value) {
  Hash *h = malloc(sizeof(Hash));
  h->key = key;
  h->value = value;
  h->next = *r;
  *r = h;
}

char *HashGet(Hash *r, char *key) {
  for (Hash *c = r; c != NULL; c = c->next) {
    if (strcmp(c->key, key) == 0)
      return c->value;
  }

  return NULL;
}

void HashPrint(Hash *r) {
  for (Hash *c = r; c != NULL; c = c->next)
    printf("%s -> %s\n", c->key, c->key);
}

void HashFree(Hash *r) {
  for (Hash *c = r; c != NULL;) {
    Hash *n = c->next;
    free(c);
    c = n;
  }
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

  int size = strlen(key)+1+strlen(escaped_value)+1; // key + = + value + \0
  char *s = malloc(size);
  sprintf(s, "%s=%s", key, escaped_value);

  curl_free(escaped_value);

  return s;
}

char *CookieUnserialize(const char *value) {
  int size;
  char *unescaped_value = curl_easy_unescape(NULL, value, 0, &size);

  char *s = malloc(size+1);
  strcpy(s, unescaped_value);

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

void Render(Response *w, const char *path, ...) {
  char *view = FileContent(path);
  if ( view == NULL ) return;

  char *header = FileContent("views/header.html");
  if ( header == NULL) {
    free(view);
    return;
  }

  char *footer = FileContent("views/footer.html");
  if ( footer == NULL ) {
    free(view);
    free(header);
    return;
  }

  char *page = calloc(strlen(view)+strlen(header)+strlen(footer)+1, sizeof(char));
  sprintf(page, "%s%s%s", header, view, footer);

  free(view);
  free(header);
  free(footer);

  w->status = 200;
  WriteHeader(w, "Content-Type", "text/html");
  w->body = page;
  w->freebody = true;
}

char *h(char *input) {
  // TODO escape input string and return new string
  return input;
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

UserID EnsureUser(Response *w, const Request *r) {
  char *userid = HashGet(r->cookie, "userid");
  if( userid == NULL ) {
    char *path = malloc(strlen(r->path)+1);
    MemoryTrack(&w->memory, path);

    strcpy(path, r->path);
    HashSet(&w->cookie, "back", path);

    Redirect(w, "/username");
    return NULL;
  }

  return userid;
}

Board *EnsureBoard(Response *w, const Request *r) {
  // TODO if board ID from request doesn't exist in memory redirect to / and return null, if board exist return it
  return NULL;
}

UserID NewUserID() {
  uuid_t uuid;
  uuid_generate(uuid);
  char *userid = malloc(UUID_STR_LEN);
  uuid_unparse_lower(uuid, userid);
  return userid;
}

char *ParamsGet(const Request *r, const char *key) {
  // TODO get query parameter value for key
  return NULL;
}

// Handlers
// =========

void RootHandler(Response *w, const Request *r) {
  if ( EnsureUser(w, r) == NULL ) return;

  Render(w, "views/index.html");
}

void GetUsernameHandler(Response *w, const Request *r) {
  HashPrint(r->cookie);
  Render(w, "views/username.html");
}

void PostUsernameHandler(Response *w, const Request *r) {
  if (HashGet(r->cookie, "userid") == NULL) {
    HashSet(&w->cookie, "username", HashGet(r->body, "username"));
    UserID userid = NewUserID();
    MemoryTrack(&w->memory, userid);
    HashSet(&w->cookie, "userid", userid);
  }

  char *back = HashGet(r->cookie, "back");
  if ( back == NULL ) return Redirect(w, "/");
  Redirect(w, back);
}

void GetBoardHandler(Response *w, const Request *r) {
  // TODO
}

void PostBoardsHandler(Response *w, const Request *r) {
  // TODO
}

void GetBoardEditHandler(Response *w, const Request *r) {
  // TODO
}

void PostBoardEditHandler(Response *w, const Request *r) {
  // TODO
}

void GetBoardResetHandler(Response *w, const Request *r) {
  // TODO
}

void GetBoardVoteHandler(Response *w, const Request *r) {
  // TODO
}

void PostBoardVoteHandler(Response *w, const Request *r) {
  // TODO
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

// Setup to invoke router
// ==================================================================

enum MHD_Result post_iterator(void *cls, enum MHD_ValueKind kind,
                              const char *key, const char *filename,
                              const char *content_type,
                              const char *transfer_encoding, const char *data,
                              uint64_t off, size_t size) {
  Request *request = cls;

  char *k = malloc(strlen(key)+1);
  MemoryTrack(&request->memory, k);

  char *v = malloc(size + 1);
  MemoryTrack(&request->memory, v);

  memcpy(k, key, strlen(key) + 1);
  memcpy(v, data, size);
  v[size] = 0;

  HashSet(&request->body, k, v);

  return MHD_YES;
}

enum MHD_Result cookie_iterator(void *cls, enum MHD_ValueKind kind,
                                const char *key, const char *value) {

  Request *r = cls;
  char *unserialized_value = CookieUnserialize(value);
  MemoryTrack(&r->memory, unserialized_value);

  char *k = malloc(strlen(key) + 1);
  MemoryTrack(&r->memory, k);
  strcpy(k, key);

  HashSet(&r->cookie, k, unserialized_value);

  return MHD_YES;
}

void request_completed(void *cls, struct MHD_Connection *connection,
                       void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct Request *r = *con_cls;

  if (NULL == r) return;

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

  MHD_get_connection_values(connection, MHD_COOKIE_KIND, &cookie_iterator, r);

  Response *w = (Response *)calloc(1, sizeof(Response));

  Router(w, r);
  printf("%d %s ... %d\n", r->method, r->path, w->status);

  if (w->status == 0) w->status = 204; // no content

  enum MHD_ResponseMemoryMode freebody =
      (w->freebody) ? MHD_RESPMEM_MUST_FREE : MHD_RESPMEM_PERSISTENT;

  int size = (w->body == NULL) ? 0 : strlen(w->body);

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

  printf("Server started on port %d\n", port);
  getc(stdin);
  MHD_stop_daemon(d);
  curl_easy_cleanup(curl);
  return 0;
}
