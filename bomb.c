#include "bomb.h"
#include "utlist.h"

BOMB *bomb_list = NULL;

/* Create a bomb in the game engine and arm it */
BOMB *spawn_bomb(cpVect position){
  BOMB *new_bomb = calloc(1, sizeof(BOMB));

  if(new_bomb == NULL){
    return NULL;
  }

  DL_APPEND(bomb_list, new_bomb);

  new_bomb->bomb = add_entity_mobile(position, BOMB_RADIUS, BOMB_MASS, 0.0, 4, RENDER_NUM_LAYERS - 2);
  new_bomb->bomb_activator = add_entity_mobile(position, BOMB_SPLASH * 20.0, 1.0, 0.0, 0, RENDER_NUM_LAYERS - 2);
  bind_bomb_trigger(new_bomb->bomb_activator);
  bind_bomb_kaboom(new_bomb->bomb);
  new_bomb->bind_with_activator = cpSpaceAddConstraint( phys_space, cpPinJointNew(new_bomb->bomb->body, new_bomb->bomb_activator->body, cpv(0.0, 0.0), cpv(0.0, 0.0)));
  new_bomb->health = 255;

  cpBodySetUserData(new_bomb->bomb->body, new_bomb);

  return new_bomb;
}
