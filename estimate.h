#pragma once
#include <stdbool.h>
#include <time.h>

#define POSTBUFFERSIZE 1024

typedef enum Method {
  METHOD_INVALID,
  METHOD_GET,
  METHOD_POST
} Method;

typedef int Status;

typedef struct Hash {
  char *key;
  void *value;
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

typedef char *UUID;

typedef struct Vote {
  struct Vote *next;
} Vote;

typedef struct Board {
  UUID id;
  UUID userid;
  char *options_str;
  bool hidden;
  Vote *votes;
  int votes_count;
  time_t updated_at;
} Board;

char *h(char *);
