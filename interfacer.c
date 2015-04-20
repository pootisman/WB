#include "game_phys.h"
#include "pre_menu.h"
#include "interfacer.h"
#include "utlist.h"

/* What interfacer does:
 * Provides us with convenient way to add entities to the game
 * We can add player, NPC, platform, trigger and text to the game field
 */
BITMAP_PLAIN *plaint_bitmap_list = NULL;
unsigned int bitmap_counter = 0;

ENT_PHYS_DYNAMIC *dynamic_phys_body_list = NULL;
unsigned int dynamic_phys_body_count = 0;

ENT_PHYS_STATIC *static_phys_body_list = NULL;
unsigned int static_phys_body_count = 0;

ENT_NOPHYS_DYNAMIC *dynamic_nophys_body_list = NULL;
unsigned int dynamic_nophys_body_count = 0;

ENT_NOPHYS_STATIC *static_nophys_body_list = NULL;
unsigned int static_nophys_body_count = 0;

ENT_NOPHYS_TEXT *nophys_text_list = NULL;
unsigned int nophys_text_count = 0;

ENT_NOPHYS_PROGBAR *nophys_progress_list = NULL;
unsigned int nophys_progress_count = 0;

ENT_PHYS_TRIGGER *phys_trigger_list = NULL;
unsigned int phys_trigger_count = 0;

ENT_NODE *ent_top = NULL;

/* Add a new movable entity to our game, inform chipmunk and allocate bitmap */
ENT_PHYS_DYNAMIC *add_entity_mobile(cpVect position, double radius, double mass, unsigned int bitmap, unsigned char layer){
  ENT_PHYS_DYNAMIC *new_node = calloc(1, sizeof(ENT_PHYS_DYNAMIC));
  BITMAP_PLAIN *temp = plaint_bitmap_list;
  double moment = cpMomentForCircle(mass, 0.0, radius, cpvzero);
  unsigned int i = 0;

  DL_APPEND(dynamic_phys_body_list, new_node);
  dynamic_phys_body_count++;

  new_node->body = cpSpaceAddBody(phys_space, cpBodyNew(mass, moment));
  new_node->shape = cpSpaceAddShape(phys_space, cpCircleShapeNew(new_node->body, radius, cpvzero));
  cpBodySetMoment(new_node->body, INFINITY);
  cpBodySetPosition(new_node->body, position);
  cpShapeSetFriction(new_node->shape, 0.2);
  cpShapeSetElasticity(new_node->shape, 1.0);
  cpShapeSetCollisionType(new_node->shape, PLAYER_COLLISION);
  new_node->layer = layer;
  new_node->body_width = radius * 2.0;
  new_node->body_height = radius * 2.0;

  if(bitmap != 0){
    for(i = 1; i < bitmap; temp = temp->next, ++i);
    new_node->bitmap = temp->bitmap;
  }

  return new_node;
}

/* Add a static (non-movable) entity to our game */
ENT_PHYS_STATIC *add_entity_static(cpVect position, double width, double height, unsigned int bitmap, unsigned char layer){
  ENT_PHYS_STATIC *new_node = calloc(1, sizeof(ENT_PHYS_STATIC));
  BITMAP_PLAIN *temp = plaint_bitmap_list;
  unsigned int i = 0;
  DL_APPEND(static_phys_body_list, new_node);
  static_phys_body_count++;

  new_node->body = cpBodyNewStatic();
  new_node->shape = cpBoxShapeNew(new_node->body, width, height, 0.1);
  cpBodySetPosition(new_node->body, position);

  cpSpaceAddShape(phys_space, new_node->shape);
  cpShapeSetFriction(new_node->shape, 0.1);
  cpShapeSetElasticity(new_node->shape, 0.9);
  cpShapeSetCollisionType(new_node->shape, PLATFORM_COLLISION);
  new_node->layer = layer;

  new_node->body_width = width;
  new_node->body_height = height;

  if(bitmap != 0){
    for(i = 1; i < bitmap; temp = temp->next, ++i);
    new_node->bitmap = temp->bitmap;
  }

  return new_node;
}

/* Bind a trigger to the specific node */
int bind_trigger(ENT_PHYS_STATIC *node, cpBool collision){

  if(node == NULL){
    (void)puts("Trigger without node, not gonna happen!");
    return -2;
  }

  cpShapeSetCollisionType(node->shape, TRIGGER_COLLISION);

  if(collision != cpTrue){
    cpShapeSetSensor(node->shape, cpTrue);
  }else{
    cpShapeSetSensor(node->shape, cpFalse);
  }

  return 0;
}

/* Add a string object to our list of rendered stuff (Copy data from pointer) */
ENT_NOPHYS_TEXT *add_entity_text(cpVect position, char *string, unsigned char layer){
  ENT_NOPHYS_TEXT *new_node = NULL;

  if(string){
    new_node = calloc(1, sizeof(ENT_NOPHYS_TEXT));
    new_node->string = calloc(strlen(string), sizeof(char));
    memcpy(new_node->string, string, strlen(string));
    new_node->position_x = position.x;
    new_node->position_y = position.y;
    new_node->layer = layer;
    new_node->is_a_copy = cpTrue;
    DL_APPEND(nophys_text_list, new_node);
    nophys_text_count++;
  }

  return new_node;
}

/* Add a string object to our list of rendered stuff (Do not copy data from pointer) */
ENT_NOPHYS_TEXT *add_entity_text_direct(cpVect position, char *string, unsigned char layer){
  ENT_NOPHYS_TEXT *new_node = NULL;

  if(string){
    new_node = calloc(1, sizeof(ENT_NOPHYS_TEXT));
    new_node->string = string;
    new_node->position_x = position.x;
    new_node->position_y = position.y;
    new_node->layer = layer;
    new_node->is_a_copy = cpFalse;
    DL_APPEND(nophys_text_list, new_node);
    nophys_text_count++;
  }

  return new_node;
}

/* Add a node without any phtsical parameters */
ENT_NOPHYS_STATIC *add_entity_nophys(cpVect position, double width, double height, unsigned int bitmap, unsigned char layer){
  ENT_NOPHYS_STATIC *new_node = NULL;
  BITMAP_PLAIN *temp = plaint_bitmap_list;
  unsigned int i = 0;

  if(bitmap != 0){
    new_node = calloc(1, sizeof(ENT_NOPHYS_STATIC));
    for(i = 1; i < bitmap; temp = temp->next, ++i);
    new_node->bitmap = temp->bitmap;
    new_node->layer = layer;
    new_node->position_x = position.x;
    new_node->position_y = position.y;
    DL_APPEND(static_nophys_body_list, new_node);
    static_nophys_body_count++;
  }

  return new_node;
}

/* Add a progressbar entity */
ENT_NOPHYS_PROGBAR *add_entity_bar(cpVect position, double length, double height, double *monitored_value_pointer, unsigned char layer){
  ENT_NOPHYS_PROGBAR *new_node = NULL;

  if(monitored_value_pointer){
    new_node = calloc(1, sizeof(ENT_NODE));
    new_node->height = height;
    new_node->length = length;
    new_node->position_x = position.x;
    new_node->position_y = position.y;
    new_node->monitored_value = monitored_value_pointer;
    new_node->layer = layer;
    DL_APPEND(nophys_progress_list, new_node);
    nophys_progress_count++;
  }

  return new_node;
}

/* Remove entity with specififc address */
void del_entity(ENT_NODE *target){

  if(target && ent_top){
    DL_DELETE(ent_top, target);
    cpSpaceRemoveShape(phys_space, target->shape);
    /* We did not add static bodies to space */
    if(cpBodyGetType(target->body) != CP_BODY_TYPE_STATIC){
      cpSpaceRemoveBody(phys_space, target->body);
    }
    cpShapeFree(target->shape);
    cpBodyFree(target->body);
  }

}

/* Free all resources and shut down the engines */
void stop_interfacer(void){
  ENT_PHYS_DYNAMIC *temp_phys_dynamic = NULL;
  ENT_PHYS_STATIC *temp_phys_static = NULL;
  ENT_NOPHYS_DYNAMIC *temp_nophys_dynamic = NULL;
  ENT_NOPHYS_STATIC *temp_nophys_static = NULL;
  ENT_NOPHYS_TEXT *temp_nophys_text = NULL;
  ENT_NOPHYS_PROGBAR *temp_nophys_progbar = NULL;
  BITMAP_PLAIN *temp_bitmap = NULL;

  DL_FOREACH(dynamic_phys_body_list, temp_phys_dynamic){
    DL_DELETE(dynamic_phys_body_list, temp_phys_dynamic);
    cpSpaceRemoveBody(phys_space, temp_phys_dynamic->body);
    cpSpaceRemoveShape(phys_space, temp_phys_dynamic->shape);
    free(temp_phys_dynamic);
  }

  DL_FOREACH(static_phys_body_list, temp_phys_static){
    DL_DELETE(static_phys_body_list, temp_phys_static);
    cpSpaceRemoveShape(phys_space, temp_phys_static->shape);
    free(temp_phys_static);
  }

  DL_FOREACH(dynamic_nophys_body_list, temp_nophys_dynamic){
    DL_DELETE(dynamic_nophys_body_list, temp_nophys_dynamic);
    free(temp_nophys_dynamic);
  }

  DL_FOREACH(static_nophys_body_list, temp_nophys_static){
    DL_DELETE(static_nophys_body_list, temp_nophys_static);
    free(temp_nophys_static);
  }

  DL_FOREACH(nophys_text_list, temp_nophys_text){
    DL_DELETE(nophys_text_list, temp_nophys_text);
    free(temp_nophys_text);
  }

  DL_FOREACH(nophys_progress_list, temp_nophys_progbar){
    DL_DELETE(nophys_progress_list, temp_nophys_progbar);
    free(temp_nophys_progbar);
  }

  DL_FOREACH(plaint_bitmap_list, temp_bitmap){
    DL_DELETE(plaint_bitmap_list, temp_bitmap);
  }

}
