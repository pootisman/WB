#ifndef _PLAYER_
#define _PLAYER_

#include <allegro5/allegro.h>
#include "common_vars.h"
#include "game_loop.h"
#include "interfacer.h"

#define DEF_PLAYER_RADIUS 16.0
#define DEF_PLAYER_MASS 32.0

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

/* Harm applied after falling into a hole */
static cpBool get_hurt(cpArbiter *arbiter, cpSpace *space, void *data){
  single_player.health -= 20;
  return cpTrue;
}

/* We reached the end the level, tell game_loop to regenerate all stuff */
static cpBool reach_teleport(cpArbiter *arbiter, cpSpace *space, void *data){
  did_reach_teleport = cpTrue;
  return cpTrue;
}

/* Die if there is no shield */
static cpBool die_now(cpArbiter *arbiter, cpSpace *space, void *data){
  single_player.health = 0;
  return cpTrue;
}

/* Player can withstand multiple hits with a shield pickup */
static cpBool buff(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;

  if(single_player.buffed >= UCHAR_MAX){
    return cpTrue;
  }

  single_player.buffed++;
  bodyA = calloc(1, sizeof(cpBody *));
  bodyB = calloc(1, sizeof(cpBody *));

  /* Remove pickup from game */
  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  /* SHHIIIIHHS */
  if(single_player.body == *bodyA){
    cpBodySetPosition(*bodyB, cpv(9999,99999));
  }else{
    cpBodySetPosition(*bodyA, cpv(9999,99999));
  }

  return cpTrue;
}

void load_skin(char *file);
PLAYER *spawn_player(cpBody *player_body);

#endif
