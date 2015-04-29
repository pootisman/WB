#ifndef _INTERCSREEN_
#define _INTERSCREEN_

#include "common_vars.h"

#define STORE_VOLUME 10

typedef struct SCORE_ENTRY{
  char name[MAX_NAME_LEN + 1];
  unsigned int score;
}SCORE_ENTRY;

/* Load previous scores from disk and  */
void prepare_scores(unsigned int current_score, char *current_name);
#endif
