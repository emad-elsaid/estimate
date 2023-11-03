#include "models.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

void BoardVotesFree(Board *b) {
  for(Vote *v = b->votes; v != NULL;) {
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

  if( old != NULL ) {
    free(old);
  }
}

bool BoardUserVoted(Board *board, UUID userid) {
  for (Vote *v = board->votes; v != NULL; v = v->next)
    if (strcmp(v->user, userid) == 0)
      return true;

  return false;
}
