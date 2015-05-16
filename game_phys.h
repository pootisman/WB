#ifndef _GAME_PHYS_
#define _GAME_PHYS_

#include <chipmunk/chipmunk.h>

#define DEF_TIME_STEP 20.0/1000.0 /* Time step for physics and Allegro engine */

/* Default gravity, set to something more interesting */
#define GRAVITY 9.8

/* 32 bit defines for collision types */
#define PLAYER_COLLISION   0x0000
#define PLATFORM_COLLISION 0x000F
#define TRIGGER_COLLISION  0x00F0
#define POWERUP_COLLISION  0x0F00
#define ENDLEVEL_COLLISION  0xF000
#define DEATHWALL_COLLISION  0xF00F
#define BOMB_ACTIVATOR_COLLISION 0xF0F0
#define BOMB_KABOOM_COLLISION 0xF0FF
#define SPPOWERUP_COLLISION 0xFF00
#define WHITE_PICKUP_COLLISION 0xFF0F
#define WHITE_COLLISION 0xFFF0
#define LASER_PICKUP_COLLISION 0xFFFF
#define LASER_COLLISION 0x000E

extern cpSpace *phys_space;
extern cpVect gravity;
extern cpShape *bed;
extern cpCollisionHandler *gap_of_death;

/* Prepares the physics for us */
int init_phys(void);
void set_gravity(cpVect new_gravity);
void stop_phys(void);

#endif
