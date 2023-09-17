#include <microhttpd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define PAGE "Hello world"

typedef enum HTTP_METHOD {
  HTTP_METHOD_INVALID,
  HTTP_METHOD_GET,
  HTTP_METHOD_POST
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

void root(Response *w, const Request *r) {
  w->body = PAGE;
  w->status = MHD_HTTP_OK;
}

Method StringToHTTPMethod(const char *method) {
  if (strcmp(method, "GET") == 0)
    return HTTP_METHOD_GET;

  if (strcmp(method, "POST") == 0)
    return HTTP_METHOD_POST;

  return HTTP_METHOD_INVALID;
}

bool PathIs(const Request *r, const char *path) {
  return strcmp(r->path, path) == 0;
}

void Router(Response *w, const Request *r) {
  bool is_GET = r->method == HTTP_METHOD_GET;
  bool is_POST = r->method == HTTP_METHOD_POST;

  if (is_GET && PathIs(r, "/")) return root(w, r);
}

static enum MHD_Result AccessCallback(void *cls, struct MHD_Connection *connection,
                                const char *url, const char *method_str,
                                const char *version, const char *upload_data,
                                size_t *upload_data_size, void **ptr) {

  Method method = StringToHTTPMethod(method_str);

  // unexpected method
  if (method == HTTP_METHOD_INVALID)
    return MHD_NO;

  // upload data in a GET!?
  if (method == HTTP_METHOD_GET && *upload_data_size != 0)
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
