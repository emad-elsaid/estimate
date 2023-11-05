#include "models.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

void BoardVotesFree(Board *b) {
  for (Vote *v = b->votes; v != NULL;) {
    Vote *n = v->next;
    free(v->vote);
    free(v->user);
    free(v);
    v = n;
  }
  b->votes = NULL;
  b->votes_count = 0;
}

void BoardTouch(Board *b) {
  char *old = b->updated_at;
  time_t t = time(NULL);
  char *buffer = malloc(30);
  sprintf(buffer, "%ld", t);
  b->updated_at = buffer;

  if (old != NULL) {
    free(old);
  }
}

bool BoardUserVoted(Board *board, UUID userid) {
  for (Vote *v = board->votes; v != NULL; v = v->next)
    if (strcmp(v->user, userid) == 0)
      return true;

  return false;
}

void BoardFreeOptions(Option *options) {
  for (Option *option = options; option != NULL; ) {
    Option *next = option->next;
    free(option->value);
    free(option);
    option = next;
  }
}

void BoardFree(Board *b) {
  free(b->id);
  free(b->options_str);
  free(b->userid);
  free(b->updated_at);

  for(Vote *v = b->votes; v!=NULL;){
    Vote *n = v->next;
    free(v->vote);
    free(v->user);
    free(v);
    v = n;
  }

  for(Hash *h=b->votes_stats; h!=NULL;){
    Hash *n = h->next;
    free(n->key);
    h = n;
  }

  HashFree(b->votes_stats);
  BoardFreeOptions(b->options);
}

void BoardSetOptions(Board *board, char *options_str) {
  if (options_str == NULL)
    return;

  board->options_str = strdup(options_str);
  Option *old_options = board->options;
  char *saveptr;
  char *val = strtok_r(options_str, "\r\n", &saveptr);

  Option *new_options = NULL;
  Option *last_option = NULL;
  while (val != NULL && *val != 0) {
    Option *opt = calloc(1, sizeof(Option));
    opt->value = strdup(val);
    opt->next = NULL;
    if (new_options == NULL) {
      new_options = opt;
    }
    if (last_option != NULL) {
      last_option->next = opt;
    }
    last_option = opt;

    val = strtok_r(NULL, "\r\n", &saveptr);
  }

  board->options = new_options;
  BoardFreeOptions(old_options);
}
