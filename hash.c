#include "hash.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
