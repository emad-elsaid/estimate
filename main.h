#pragma once
#include "hash.h"
#include <stdbool.h>

#define POSTBUFFERSIZE 1024

typedef enum Method { METHOD_INVALID, METHOD_GET, METHOD_POST } Method;

typedef int Status;

typedef struct Memory {
  void *memory;
  struct Memory *next;
} Memory;

typedef struct Request {
  Method method;
  const char *path;
  Hash *params;
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
