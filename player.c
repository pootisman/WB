#include <string.h>
#include "game_phys.h"
#include "player.h"
#include "common_vars.h"

PLAYER single_player;

void load_skin(char *file){
  if(file != NULL){
    bcopy(file, &(single_player.skin[0]), 32);
  }else{
    return;
  }
}

PLAYER *spawn_player(cpBody *player_body){
  single_player.radius = DEF_PLAYER_RADIUS;
  single_player.mass = DEF_PLAYER_MASS;
  single_player.body = player_body;
  single_player.health = 255;
  single_player.spPwup = 0;
  return &single_player;
}
