#ifndef _GAME_PHYS_
#define _GAME_PHYS_

#include <chipmunk/chipmunk.h>

#define DEF_TIME_STEP 50.0/1000.0 /* Time step for physics and Allegro engine */

#define GRAVITY 9.8

/* 32 bit defines for collision types */
#define PLAYER_COLLISION   0x0000
#define PLATFORM_COLLISION 0x000F
#define TRIGGER_COLLISION  0x00F0
#define POWERUP_COLLISION  0x0F00
#define ENDLEVEL_COLLISION  0xF000

extern cpSpace *phys_space;
extern cpVect gravity;
extern cpShape *bed;
extern cpCollisionHandler *gap_of_death;

int init_phys(void);
void set_gravity(cpVect new_gravity);
void stop_phys(void);

#endif
