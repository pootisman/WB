#ifndef _BOMB_
#define _BOMB_
#include <chipmunk/chipmunk.h>
#include "interfacer.h"
#include "render.h"
#include "player.h"

#define BOMB_RADIUS 8.0
#define BOMB_SPLASH 16.0
#define BOMB_MASS 64.0

typedef struct BOMB{
  cpVect target;
  cpShape *bomb_shape;
  cpConstraint *bind_with_activator;
  ENT_PHYS_DYNAMIC *bomb_activator;
  cpBody *bomb_body;
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

#endif
