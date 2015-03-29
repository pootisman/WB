#ifndef _PLAYER_
#define _PLAYER_

#include <allegro5/allegro.h>
#include <chipmunk/chipmunk.h>
#include "common_vars.h"

#define DEF_PLAYER_RADIUS 16.0
#define DEF_PLAYER_MASS 70.0

typedef struct PLAYER{
  char skin[32];
  int health;
  unsigned char buffed;
  double radius;
  double mass;
  char player_name[MAX_NAME_LEN + 1];
  unsigned char name_length;
  cpBody *body;
}PLAYER;

extern PLAYER single_player;

static cpBool get_hurt(cpArbiter *arbiter, cpSpace *space, void *data){
  single_player.health -= 20;
  return cpTrue;
}

void load_skin(char *file);
PLAYER *spawn(cpBody *player_body);

#endif
