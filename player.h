#ifndef _PLAYER_
#define _PLAYER_

#include <allegro5/allegro.h>
#include "common_vars.h"
#include "game_loop.h"
#include "interfacer.h"

#define DEF_PLAYER_RADIUS 16.0
#define DEF_PLAYER_MASS 32.0
#define DEF_PLAYER_BONUS_STEP 1000


typedef struct PLAYER{
  char skin[32];
  int health;
  unsigned int score;
  unsigned char buffed, charge, laser, grav;
  double radius;
  double mass;
  char player_name[MAX_NAME_LEN + 1];
  unsigned char name_length;
  struct timeval hole_cooldown;
  cpBool has_white_hole, has_laser;
  int spPwup;
  cpBody *body;
  ENT_PHYS_DYNAMIC *weapon_range;
}PLAYER;

extern PLAYER single_player;

/* Harm applied after falling into a hole */
static cpBool get_hurt(cpArbiter *arbiter, cpSpace *space, void *data){
  single_player.health -= 20;
  return cpTrue;
}

/* We reached the end the level, tell game_loop to regenerate all stuff */
static cpBool reach_teleport(cpArbiter *arbiter, cpSpace *space, void *data){
  extern unsigned int level_counter;
  if(single_player.spPwup >= 20 * level_counter){
    did_reach_teleport = cpTrue;
  }
  return cpTrue;
}

/* Die if there is no shield */
static cpBool die_now(cpArbiter *arbiter, cpSpace *space, void *data){
  single_player.health = 0;
  return cpTrue;
}

/* Hit special power-ups */
static cpBool hit_spPwup(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;

  single_player.spPwup += 20;
  single_player.score += 20;

  bodyA = calloc(1, sizeof(cpBody *));
  bodyB = calloc(1, sizeof(cpBody *));

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(single_player.body == *bodyA){
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyB));
  }else{
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyA));
  }

  return cpTrue;
}

/* Player can withstand multiple hits with a shield pickup */
static cpBool buff(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;


  if(single_player.buffed >= UCHAR_MAX){
    return cpTrue;
  }

  single_player.buffed++;
  single_player.score += 20;
  bodyA = calloc(1, sizeof(cpBody *));
  bodyB = calloc(1, sizeof(cpBody *));

  /* Remove pickup from game */
  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  /* NO SHHIIIIHHS */
  if(single_player.body == *bodyA){
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyB));
  }else{
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyA));
  }

  return cpTrue;
}

/* Player can pickup a laser */
static cpBool pick_laser(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;

  bodyA = calloc(1, sizeof(cpBody *));
  bodyB = calloc(1, sizeof(cpBody *));

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(single_player.body == *bodyA){
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyB));
  }else{
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyA));
  }

  single_player.has_laser = cpTrue;
  single_player.charge = UCHAR_MAX;

  return cpTrue;
}

/* Player can pickup a laser */
static cpBool pick_whole(cpArbiter *arbiter, cpSpace *space, void *data){
  cpBody **bodyA, **bodyB;

  bodyA = calloc(1, sizeof(cpBody *));
  bodyB = calloc(1, sizeof(cpBody *));

  cpArbiterGetBodies(arbiter, bodyA, bodyB);

  if(single_player.body == *bodyA){
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyB));
  }else{
    remove_ent_phy_dyn(cpBodyGetUserData(*bodyA));
  }

  single_player.has_white_hole = cpTrue;
  single_player.charge = UCHAR_MAX;

  return cpTrue;
}

void load_skin(char *file);
PLAYER *spawn_player(cpBody *player_body);

#endif
