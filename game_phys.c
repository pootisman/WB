#include <stdio.h>
#include "utlist.h"
#include "game_phys.h"
#include "player.h"
#include "common_vars.h"

cpSpace *phys_space = NULL;
cpVect gravity;
cpShape *bed = NULL, *player_shape = NULL, *static_shape = NULL;
cpBody *player_body = NULL;
cpFloat player_mass, player_radius, player_moment;
cpCollisionHandler *gap_of_death = NULL, *amp = NULL;
cpSpaceHash *space_hash = NULL;

/* Prepare space for our game */
int init_phys(){
  (void)fputs("[cpSpaceNew>--", stdout);

  phys_space = cpSpaceNew();

  cpSpaceUseSpatialHash(phys_space, 128.0, 1024);

  if(phys_space == NULL){
    (void)puts("--<FAILED]");
    return -1;
  }

  (void)puts("--<DONE]");

  gravity = cpv(0.0, GRAVITY);
  
  cpSpaceSetGravity(phys_space, gravity);
  
  (void)fputs("[Adding collision handlers>--", stdout);
  
  gap_of_death = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, TRIGGER_COLLISION);
  
  amp = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, POWERUP_COLLISION);
  if(amp == NULL || gap_of_death == NULL){
    (void)puts("--<FAILED]");
    return -2;
  }
  (void)puts("--<DONE]");

  gap_of_death->preSolveFunc = get_hurt;

  return 0;
}

/* Set gravity direction */
void set_gravity(cpVect new_gravity){
  cpSpaceSetGravity(phys_space, new_gravity);
}

/* Free all physics that we currently have loaded */
void stop_phys(){
  (void)puts("Shutting down physics.");
  cpSpaceFree(phys_space);
  (void)puts("Physics shutdown successful.");
}