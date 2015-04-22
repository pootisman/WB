#ifndef _GAME_LOOP_
#define _GAME_LOOP_

#include <chipmunk/chipmunk.h>

extern cpBool did_reach_teleport;

void init_critical(void);
void init_level(void);
void gen_chunk(void);

int run_loop(void);

void stop_game(void);

#endif
