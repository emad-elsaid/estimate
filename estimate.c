#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

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

typedef struct Cookie {
  char *username;
  char *userid;
  char *back;
} Cookie;

typedef struct Request {
  Method method;
  const char *path;
  Cookie *cookie;
  Hash *body;
  // This section is for MHD variables
  struct MHD_PostProcessor *postprocessor;
} Request;

typedef struct Response {
  Status status;
  void *body;
  Hash *headers;
  bool freebody;
  Cookie *cookie;
} Response;

typedef char *UserID;

typedef struct Board {

} Board;

// Helpers
// ===================================================================

Hash *HashSet(Hash *r, char *key, char *value) {
  Hash *h = malloc(sizeof(Hash));
  h->key = key;
  h->value = value;
  h->next = r;
  return h;
}

void HashFree(Hash *r, bool freeKey, bool freeValue) {
  for(Hash *c = r; c != NULL;){
    Hash *n = c->next;

    if(freeKey) free(c->key);
    if(freeValue) free(c->value);
    free(c);

    c = n;
  }
}

void WriteHeader(Response *w, char *key, char *value) {
  w->headers = HashSet(w->headers, key, value);
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

UserID EnsureUser(Response *w, const Request *r) {
  // TODO if user doesn't exist redirect to /username and return null, if user exist return userID
  return NULL;
}

Board *EnsureBoard(Response *w, const Request *r) {
  // TODO if board ID from request doesn't exist in memory redirect to / and return null, if board exist return it
  return NULL;
}

UserID NewUserID() {
  return NULL;
}

void CookieInit(Response *w) {
  if (w->cookie != NULL) return;
  w->cookie = calloc(1, sizeof(Cookie));
}

void CookieFree(Response *w) {
  if ( w->cookie == NULL ) return;
  free(w->cookie);
}

char *ParamsGet(const Request *r, const char *key) {
  // TODO get query parameter value for key
  return NULL;
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

// Handlers
// ==================================================================

void RootHandler(Response *w, const Request *r) {
  w->body = "Hello world";
  w->status = 200;
}

void GetUsernameHandler(Response *w, const Request *r) {
  Render(w, "views/username.html");
}

void PostUsernameHandler(Response *w, const Request *r) {
  /* CookieInit(w); */
  /* w->cookie->username = ParamsGet(r, "username"); */

  /* if ( r->cookie == NULL || r->cookie->userid == NULL ) { */
  /*   w->cookie->userid = NewUserID(); */
  /* } */

  w->status = 200;
  w->body = "registered";

  /* if ( r->cookie == NULL || r->cookie->back == NULL ) return Redirect(w, "/"); */

  /* Redirect(w, r->cookie->back); */
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
  struct Request *request = cls;

  if (0 == strcmp ("DONE", key)) {
    return MHD_YES;
  }

  /* if (0 == strcmp ("v1", key)) { */
  /*     if (size + off >= sizeof(session->value_1)) size = sizeof (session->value_1) - off - 1; */
  /*     memcpy (&session->value_1[off], data, size); */
  /*     session->value_1[size+off] = '\0'; */
  /*     return MHD_YES; */
  /* } */

  /* if (0 == strcmp ("v2", key)) { */
  /*     if (size + off >= sizeof(session->value_2)) size = sizeof (session->value_2) - off - 1; */
  /*     memcpy (&session->value_2[off], data, size); */
  /*     session->value_2[size+off] = '\0'; */
  /*     return MHD_YES; */
  /* } */

  return MHD_YES;
}

void request_completed(void *cls, struct MHD_Connection *connection,
                       void **con_cls, enum MHD_RequestTerminationCode toe) {
  struct Request *r = *con_cls;

  if (NULL == r) return;

  if (r->method == METHOD_POST) {
    MHD_destroy_post_processor(r->postprocessor);
    HashFree(r->body, true, true);
  }

  free(r);
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

  Response *w = (Response *)calloc(1, sizeof(Response));

  Router(w, r);
  printf("%d %s ... %d\n", r->method, r->path, w->status);

  CookieFree(w);
  if (w->status == 0) {
    free(r);
    free(w);
    return MHD_NO;
  }

  enum MHD_ResponseMemoryMode freebody =
      (w->freebody) ? MHD_RESPMEM_MUST_FREE : MHD_RESPMEM_PERSISTENT;
  struct MHD_Response *response =
      MHD_create_response_from_buffer(strlen(w->body), w->body, freebody);

  for (Hash *h = w->headers; h != NULL; h = h->next)
    MHD_add_response_header(response, h->key, h->value);

  enum MHD_Result ret = MHD_queue_response(connection, w->status, response);

  MHD_destroy_response(response);
  HashFree(w->headers, true, false);
  free(w);
  return ret;
  }

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("%s PORT\n", argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);

  struct MHD_Daemon *d = MHD_start_daemon(
      MHD_USE_AUTO_INTERNAL_THREAD, port, NULL, NULL, &AccessCallback, NULL,
      MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
      MHD_OPTION_END);
  if (d == NULL) return 1;

  printf("Server started on port %d\n", port);
  getc(stdin);
  MHD_stop_daemon(d);
  return 0;
}
