#pragma once

typedef struct Hash {
  char *key;
  void *value;
  struct Hash *next;
} Hash;

void HashSet(Hash **r, char *key, void *value);
void *HashGet(Hash *r, void *key);
void HashPrint(Hash *r);
void HashFree(Hash *r);
