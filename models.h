#pragma once
#include "hash.h"
#include <stdbool.h>

typedef char *UUID;

typedef struct Vote {
  UUID user;
  char *vote;
  struct Vote *next;
} Vote;

typedef struct Option {
  char *value;
  struct Option *next;
} Option;

typedef struct Board {
  UUID id;
  UUID userid;
  Option *options;
  char *options_str;
  bool hidden;
  Vote *votes;
  int votes_count;
  Hash *votes_stats;
  char *updated_at;
} Board;

typedef struct BoardPage {
  Board *board;
  UUID user;
} BoardPage;

void BoardVotesFree(Board *b);
void BoardTouch(Board *b);
bool BoardUserVoted(Board *board, UUID userid);
void BoardFreeOptions(Option *options);
void BoardFree(Board *b);
void BoardSetOptions(Board *board, char *options_str);
