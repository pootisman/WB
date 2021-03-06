#include <stdio.h>
#include "utlist.h"
#include "game_phys.h"
#include "player.h"
#include "common_vars.h"
#include "bomb.h"

cpSpace *phys_space = NULL;
cpVect gravity;
cpShape *bed = NULL, *player_shape = NULL, *static_shape = NULL;
cpBody *player_body = NULL;
cpFloat player_mass, player_radius, player_moment;
cpCollisionHandler *gap_of_death = NULL, *amp = NULL, *levels_end = NULL, *deathwall = NULL, *bomb_activ = NULL, *bomb_kaboom = NULL, *collect_sp = NULL, *laser_pick = NULL, *laser_shoot = NULL, *whole_pick = NULL, *whole_act = NULL;
cpSpaceHash *space_hash = NULL;

/* Prepare chipmunk space for our game */
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
  
  bomb_kaboom = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, BOMB_KABOOM_COLLISION);
  bomb_activ = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, BOMB_ACTIVATOR_COLLISION);
  deathwall = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, DEATHWALL_COLLISION);
  gap_of_death = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, TRIGGER_COLLISION);
  levels_end = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, ENDLEVEL_COLLISION);
  amp = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, POWERUP_COLLISION);
  collect_sp = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, SPPOWERUP_COLLISION);
  laser_pick = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, LASER_PICKUP_COLLISION);
  laser_shoot = cpSpaceAddCollisionHandler(phys_space, BOMB_KABOOM_COLLISION, LASER_COLLISION);
  whole_pick = cpSpaceAddCollisionHandler(phys_space, PLAYER_COLLISION, WHITE_PICKUP_COLLISION);
  whole_act = cpSpaceAddCollisionHandler(phys_space, BOMB_KABOOM_COLLISION, WHITE_COLLISION);

  if(amp == NULL || gap_of_death == NULL || levels_end == NULL){
    (void)puts("--<FAILED]");
    return -2;
  }
  (void)puts("--<DONE]");

  gap_of_death->preSolveFunc = get_hurt;
  levels_end->preSolveFunc = reach_teleport;
  deathwall->preSolveFunc = die_now;
  bomb_activ->preSolveFunc = approach_target;
  bomb_kaboom->preSolveFunc = detonate;
  amp->preSolveFunc = buff;
  collect_sp->preSolveFunc = hit_spPwup;
  laser_pick->preSolveFunc = pick_laser;
  laser_shoot->preSolveFunc = hit_by_laser;
  whole_pick->preSolveFunc = pick_whole;
  whole_act->preSolveFunc = hit_by_grav;

  return 0;
}

/* Set gravity direction, well just a wrapper =P */
void set_gravity(cpVect new_gravity){
  cpSpaceSetGravity(phys_space, new_gravity);
}

/* Free physics space that we currently have loaded */
void stop_phys(){
  (void)puts("Shutting down physics.");
  cpSpaceFree(phys_space);
  (void)puts("Physics shutdown successful.");
}
