#include "render.h"
#include "white_hole.h"
#include <chipmunk/chipmunk.h>

WHITE_HOLE *hole_list = NULL;

/* Create a white hole pickup */
WHITE_HOLE *white_hole_spawn(cpVect position){
  WHITE_HOLE *new_hole = calloc(1, sizeof(WHITE_HOLE));

  new_hole->owner = NULL;
  new_hole->body = add_entity_mobile(position, WHITE_HOLE_PICKUP_RADIUS, WHITE_HOLE_PICKUP_MASS, 0.0, 6, RENDER_NUM_LAYERS - 2);

  cpBodySetUserData(new_hole->body->body, new_hole);
  DL_APPEND(hole_list, new_hole);

  return new_hole;
}
