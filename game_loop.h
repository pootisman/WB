#ifndef _GAME_LOOP_
#define _GAME_LOOP_

#include <chipmunk/chipmunk.h>

/* Did we reach the portal to the next level? */
extern cpBool did_reach_teleport;

/* Load minimum bitmap set and reset transforms */
void init_critical(unsigned int level);
/* Spawn all platforms on the level and add NPCs */
void init_level(void);
/* Run while:
   [1] Player is alive
   [2] There is some time left*/
int run_loop(void);

/* Stop all systems of the game */
void stop_game(void);

#endif
