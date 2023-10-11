#include <string.h>
#include <stdlib.h>
#include "./string.h"

String *StringNew(char *c) {
  String *s = (String*) malloc(sizeof(String));
  int len = (c != NULL )? strlen(c) : 1;

  s->value = (c != NULL)? strdup(c) : calloc(1, 1);
  s->len = len;
  s->cap = s->len;

  return s;
}

void StringFree(String *s) {
  free(s->value);
  free(s);
}

void StringWrite(String *s, char *c) {
  if( c == NULL || *c == 0) return;

  if( s->cap == 0 ){
    s->value = strdup(c);
    s->len = strlen(c);
    s->cap = s->len;
    return;
  }

  int newlen = s->len + strlen(c);

  if( newlen > s->cap ) {
    int newcap = newlen;
    char *newvalue = (char *) malloc(newcap+1);

    strcpy(newvalue, s->value);
    strcat(newvalue, c);

    free(s->value);

    s->value = newvalue;
    s->len = newlen;
    s->cap = newcap;
  }else{
    strcat(s->value, c);
    s->len = newlen;
  }
}
