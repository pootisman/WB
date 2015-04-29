#include "interscreen.h"
#include "common_vars.h"
#include "interfacer.h"
#include "render.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Compare score records, if the score is same, sort lexicographically */
static int compare_scores(const void *Av, const void *Bv){
  SCORE_ENTRY *A = (SCORE_ENTRY *)Av;
  SCORE_ENTRY *B = (SCORE_ENTRY *)Bv;

  if(A->score > B->score){
    return -1;
  }else if(A->score < B->score){
    return 1;
  }else{
    return -strcmp(&(A->name[0]), &(B->name[0]));
  }

  return 0;
}

/* Load previous scores from disk and  */
void prepare_scores(unsigned int current_score, char *current_name){
  char score_string[MAX_NAME_LEN * 2] = {0};
  unsigned char i = 0, j = 0;
  FILE *scores_file = fopen(".highscores.dat", "rb+");
  SCORE_ENTRY scores[STORE_VOLUME + 1] = {{0}, 0};

  if(scores_file == NULL){
    scores[0].score = current_score;
    strncpy(&(scores[j].name[0]), current_name, MAX_NAME_LEN);
    scores_file = fopen(".highscores.dat", "wb+");
    (void)fwrite(&(scores[0]), sizeof(SCORE_ENTRY), STORE_VOLUME, scores_file);
    j = 1;
  }else{
    j = fread(&scores[0], sizeof(SCORE_ENTRY), STORE_VOLUME, scores_file);
    (void)printf("Read %d scores from file.\n", j);

    scores[j].score = current_score;
    strncpy(&(scores[j].name[0]), current_name, MAX_NAME_LEN);

    qsort(&(scores[0]), STORE_VOLUME + 1, sizeof(SCORE_ENTRY), compare_scores);
    (void)fseek(scores_file, 0, SEEK_SET);
    (void)fwrite(&(scores[0]), sizeof(SCORE_ENTRY), STORE_VOLUME, scores_file);
  }

  add_entity_text(cpv(renderer.view_width * 0.5, renderer.view_height * (0.2 + -1.0/STORE_VOLUME/2.0)), "HIGHSCORES", RENDER_NUM_LAYERS - 1);
  for(i = 0; i < j; i++){
    sprintf(&(score_string[0]), "%s...%d", scores[i].name, scores[i].score);

    if(scores[i].name[0] != 0){
      add_entity_text(cpv(renderer.view_width * 0.5, renderer.view_height * (0.2 + ((double)i/(double)j/2.0))), &(score_string[0]), RENDER_NUM_LAYERS - 1);
    }
  }

  (void)fclose(scores_file);
}
