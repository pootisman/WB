#ifndef _WHITE_HOLE_
#define _WHITE_HOLE_

#include "utlist.h"
#include "interfacer.h"
#include "player.h"
#include <time.h>

#define WHITE_HOLE_IMPULSE 128.0
#define WHITE_HOLE_RADIUS 256.0
#define WHITE_HOLE_COOLDOWN 30.0

#define WHITE_HOLE_PICKUP_RADIUS 8.0
#define WHITE_HOLE_PICKUP_MASS 16.0

typedef struct WHITE_HOLE{
  ENT_PHYS_DYNAMIC *body;
  PLAYER *owner;
  struct WHITE_HOLE *prev, *next;
}WHITE_HOLE;

/* Called on pickup, removes powerup from the game */
static cpBool pickup(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = NULL, **bodyB = NULL;
  WHITE_HOLE *picked = NULL;
  extern WHITE_HOLE *hole_list;

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(cpBodyGetUserData(*bodyA) == NULL){
    picked = cpBodyGetUserData(*bodyB);
  }else{
    picked = cpBodyGetUserData(*bodyA);
  }

  remove_ent_phy_dyn(picked->body);
  DL_DELETE(hole_list, picked);
  (void)free(picked);
  single_player.has_white_hole = cpTrue;
}

/* Detects whether we should apply force to other bodies */
static cpBool repell(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;
  cpVect target_pos = {.x = 0.0, .y = 0.0};

  if(single_player.hole_cooldown.tv_usec <= 0.0){
    cpArbiterGetBodies(arbiter, bodyA, bodyB);

    if(cpBodyGetUserData(*bodyA) == NULL){

    }else{

    }
  }
}

#endif

