#ifndef _BOMB_
#define _BOMB_
#include <chipmunk/chipmunk.h>
#include <stdio.h>
#include "utlist.h"
#include "game_phys.h"
#include "interfacer.h"
#include "render.h"
#include "player.h"
#include "aux.h"

#define BOMB_RADIUS 8.0
#define BOMB_SPLASH 16.0
#define BOMB_MASS 32.0
#define BOMB_ACTIVATION 256.0
#define BOMB_IMPULSE 256.0

typedef struct BOMB{
  cpVect target;
  cpConstraint *bind_with_activator;
  ENT_PHYS_DYNAMIC *bomb;
  ENT_PHYS_DYNAMIC *bomb_activator;
  int health;
  struct BOMB *prev, *next;
}BOMB;

extern BOMB *bomb_list;

BOMB *spawn_bomb(cpVect position);

/* Push bomb toward the target */
static cpBool approach_target(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  cpArbiterGetBodies(arbiter, bodyA, bodyB);

#ifdef DEBUG
  (void)puts("Bomb active.");
#endif

  if(single_player.body == *bodyA){
    cpBodyApplyImpulseAtWorldPoint(*bodyB, pointer_vect_mul(cpBodyGetPosition(*bodyA), cpBodyGetPosition(*bodyB), BOMB_IMPULSE), cpv(0.0, 0.0));
  }else{
    cpBodyApplyImpulseAtWorldPoint(*bodyA, pointer_vect_mul(cpBodyGetPosition(*bodyB), cpBodyGetPosition(*bodyA), BOMB_IMPULSE), cpv(0.0, 0.0));
  }

  free(bodyA);
  free(bodyB);

  return cpTrue;
}

/* Explosive hugs, YAY! =D */
static cpBool detonate(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  BOMB *temp = NULL;

#ifdef DEBUG
  (void)puts("Time to go BOOM!");
#endif

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
  single_player.score += 40;
  single_player.score_lives += 40;
  remove_ent_phy_dyn(temp->bomb);
  remove_ent_phy_dyn(temp->bomb_activator);

  DL_DELETE(bomb_list, temp);
  (void)free(temp);
  (void)free(bodyA);
  (void)free(bodyB);

  return cpTrue;
}

/* Get hit by a laser */
static cpBool hit_by_laser(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  BOMB *temp = NULL;

#ifdef DEBUG
  (void)puts("BOMB: Ouch");
#endif

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(cpBodyGetUserData(*bodyA) == NULL){
    temp = cpBodyGetUserData(*bodyB);
  }else{
    temp = cpBodyGetUserData(*bodyA);
  }

  if(single_player.charge - 5 > 0 && single_player.has_white_hole == cpTrue && single_player.grav != 0){
    cpBodyApplyImpulseAtWorldPoint(temp->bomb->body, cpv(-100*(cpBodyGetPosition(single_player.body).x - cpBodyGetPosition(temp->bomb->body).x), -100*(cpBodyGetPosition(single_player.body).y - cpBodyGetPosition(temp->bomb->body).y)), cpv(0.0, 0.0));
    single_player.charge -= 20;
  }


  if(single_player.charge - 5 > 0 && single_player.has_laser == cpTrue && single_player.laser != 0){
    add_entity_line(cpBodyGetPosition(*bodyA), cpBodyGetPosition(*bodyB), RENDER_NUM_LAYERS - 2);
    if(temp->health - 10 <= 0){
      single_player.score += 40;
      single_player.score_lives += 40;
      remove_ent_phy_dyn(temp->bomb);
      remove_ent_phy_dyn(temp->bomb_activator);

      DL_DELETE(bomb_list, temp);
      (void)free(temp);
      (void)free(bodyA);
      (void)free(bodyB);
    }else{
      temp->health -= 10;
    }
    single_player.charge -= 5;
  }

  return cpTrue;
}

/* Get hit by a laser */
static cpBool hit_by_grav(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA = calloc(1, sizeof(cpBody *)), **bodyB = calloc(1, sizeof(cpBody *));
  BOMB *temp = NULL;

#ifdef DEBUG
  (void)puts("Time to go BOOM!");
#endif

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(cpBodyGetUserData(*bodyA) == NULL){
    temp = cpBodyGetUserData(*bodyB);
  }else{
    temp = cpBodyGetUserData(*bodyA);
  }

  if(single_player.charge - 5 > 0 && single_player.has_white_hole == cpTrue && single_player.grav != 0){
    cpBodyApplyImpulseAtWorldPoint(temp->bomb->body, cpv(cpBodyGetPosition(single_player.body).x - cpBodyGetPosition(temp->bomb->body).x, cpBodyGetPosition(single_player.body).y - cpBodyGetPosition(temp->bomb->body).y), cpv(0.0, 0.0));
    single_player.charge -= 20;
  }

  return cpTrue;
}

#endif
