#pragma once
#include <string.h>
#include <stdlib.h>

typedef struct String {
  char *value;
  int len;
  int cap;
} String;

String *StringNew(char *c);
void StringFree(String *s);
void StringWrite(String *s, char *c);
