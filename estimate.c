#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define PAGE "Hello world"

// Data structures
// ===================================================================

typedef enum Method {
  METHOD_INVALID,
  METHOD_GET,
  METHOD_POST
} Method;

typedef int Status;

typedef struct Request {
  Method method;
  const char *path;
} Request;

typedef struct Response {
  Status status;
  void *body;
} Response;

typedef char* UserID;

typedef struct Board {

} Board;

// Helpers
// ===================================================================

Method StringToHTTPMethod(const char *method) {
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
  // TODO render path with key, value rest of params
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

void CookiesSet(Response *w, const Request *r, char *key, char *value){
  // TODO write a key/value to cookies
}

char *CookiesGet(const Request *r, char *key) {
  // TODO return cookie key value
  return NULL;
}

bool CookiesExist(const Request *r, const char *key) {
  // TODO return true if key exist in request cookies
  return false;
}

char *ParamsGet(const Request *r, const char *key) {
  // TODO get query parameter value for key
  return NULL;
}

void Redirect(Response *w, const char *path) {
  // TODO redirect user to another path
}

// Handlers
// ==================================================================

void RootHandler(Response *w, const Request *r) {
  w->body = PAGE;
  w->status = MHD_HTTP_OK;
}

void GetUsernameHandler(Response *w, const Request *r) {
  Render(w, "username");
}

void PostUsernameHandler(Response *w, const Request *r) {
  CookiesSet(w, r, "username", ParamsGet(r, "username"));
  if (! CookiesExist(r, "userid")) {
    CookiesSet(w, r, "userid", NewUserID());
  }

  Redirect(w, CookiesGet(r, "back"));
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
}

// Setup to invoke router
// ==================================================================

static enum MHD_Result AccessCallback(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method_str,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **ptr) {

  Method method = StringToHTTPMethod(method_str);

  // unexpected method
  if (method == METHOD_INVALID)
    return MHD_NO;

  // upload data in a GET!?
  if (method == METHOD_GET && *upload_data_size != 0)
    return MHD_NO;

  static int dummy;
  if (&dummy != *ptr) {
    // The first time only the headers are valid,
    // do not respond in the first round...
    *ptr = &dummy;
    return MHD_YES;
  }

  // clear context pointer
  *ptr = NULL;

  Request *r = (Request *)calloc(1, sizeof(Request));
  r->method = method;
  r->path = url;

  Response *w = (Response *)calloc(1, sizeof(Response));

  Router(w, r);
  if( w->status == 0 ) {
    free(r);
    free(w);
    return MHD_NO;
  }

  struct MHD_Response *response = MHD_create_response_from_buffer(
      strlen(w->body), w->body, MHD_RESPMEM_PERSISTENT);
  enum MHD_Result ret = MHD_queue_response(connection, w->status, response);

  MHD_destroy_response(response);

  free(r);
  free(w);
  return ret;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("%s PORT\n", argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);

  struct MHD_Daemon *d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, port, NULL, NULL, &AccessCallback, NULL, MHD_OPTION_END);
  if (d == NULL) return 1;

  printf("Server started on port %d", port);
  getc(stdin);
  MHD_stop_daemon(d);
  return 0;
}
