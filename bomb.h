#ifndef _BOMB_
#define _BOMB_
#include <chipmunk/chipmunk.h>
#include <stdio.h>
#include "game_phys.h"
#include "interfacer.h"
#include "render.h"
#include "player.h"

#define BOMB_RADIUS 8.0
#define BOMB_SPLASH 16.0
#define BOMB_MASS 32.0
#define BOMB_ACTIVATION 256.0

typedef struct BOMB{
  cpVect target;
  cpConstraint *bind_with_activator;
  ENT_PHYS_DYNAMIC *bomb;
  ENT_PHYS_DYNAMIC *bomb_activator;
  int health;
  struct BOMB *prev, *next;
}BOMB;

BOMB *spawn_bomb(cpVect position);

/* Push bomb toward the target */
static cpBool approach_target(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  (void)puts("Bomb active.");

  if(single_player.body == *bodyA){
    cpBodyApplyImpulseAtWorldPoint(*bodyB, cpv(0.5 * (cpBodyGetPosition(*bodyA).x - cpBodyGetPosition(*bodyB).x), 0.5 * (cpBodyGetPosition(*bodyA).y - cpBodyGetPosition(*bodyB).y)), cpv(0.0, 0.0));
  }else{
    cpBodyApplyImpulseAtWorldPoint(*bodyA, cpv(0.5 * (cpBodyGetPosition(*bodyB).x - cpBodyGetPosition(*bodyA).x), 0.5 * (cpBodyGetPosition(*bodyB).y - cpBodyGetPosition(*bodyA).y)), cpv(0.0, 0.0));
  }

  free(bodyA);
  free(bodyB);

  return cpTrue;
}

/* Explosive hugs */
static cpBool detonate(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  BOMB *temp = NULL;

  (void)puts("Time to go BOOM!");

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(cpBodyGetUserData(*bodyA) == NULL){
    temp = cpBodyGetUserData(*bodyB);
  }else{
    temp = cpBodyGetUserData(*bodyA);
  }

  if(single_player.buffed > 0){
    single_player.buffed--;
  }else{
    single_player.health = 0;
  }
  remove_ent_phy_dyn(temp->bomb);
  remove_ent_phy_dyn(temp->bomb_activator);

  return cpTrue;
}

#endif
