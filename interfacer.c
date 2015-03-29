#include "game_phys.h"
#include "pre_menu.h"
#include "interfacer.h"
#include "utlist.h"

/* What interfacer does:
 * Provides us with convenient way to add entities to the game
 * We can add player, NPC, platform, trigger and text to the game field
 */

ENT_NODE *ent_top = NULL;

/* Add a new movable entity to our game, inform chipmunk and allocate bitmap */
ENT_NODE *add_entity_mobile(cpVect position, double radius, double mass, char *bitmap_file, unsigned char layer){
  ENT_NODE *new_node = calloc(1, sizeof(ENT_NODE));
  double moment = cpMomentForCircle(mass, 0.0, radius, cpvzero);

  DL_APPEND(ent_top, new_node);

  new_node->body = cpSpaceAddBody(phys_space, cpBodyNew(mass, moment));
  new_node->shape = cpSpaceAddShape(phys_space, cpCircleShapeNew(new_node->body, radius, cpvzero));
  cpBodySetMoment(new_node->body, INFINITY);
  cpBodySetPosition(new_node->body, position);
  cpShapeSetFriction(new_node->shape, 0.2);
  cpShapeSetElasticity(new_node->shape, 1.0);
  cpShapeSetCollisionType(new_node->shape, PLAYER_COLLISION);
  new_node->body_height = 2.0 * radius;
  new_node->body_width = 2.0 * radius;
  new_node->layer = layer;

  if(bitmap_file){
    new_node->bitmap = al_load_bitmap(bitmap_file);
    new_node->bitmap_height = al_get_bitmap_height(new_node->bitmap);
    new_node->bitmap_width = al_get_bitmap_width(new_node->bitmap);
  }

  return new_node;
}

/* Add a static (non-movable) entity to our game */
ENT_NODE *add_entity_static(cpVect position, double width, double height, char *bitmap_file, unsigned char layer){
  ENT_NODE *new_node = calloc(1, sizeof(ENT_NODE));

  DL_APPEND(ent_top, new_node);

  new_node->body = cpBodyNewStatic();
  new_node->shape = cpBoxShapeNew(new_node->body, width, height, 0.1);
  cpBodySetPosition(new_node->body, position);

  cpSpaceAddShape(phys_space, new_node->shape);
  cpShapeSetFriction(new_node->shape, 0.1);
  cpShapeSetElasticity(new_node->shape, 0.9);
  cpShapeSetCollisionType(new_node->shape, PLATFORM_COLLISION);
  new_node->body_height = height;
  new_node->body_width = width;
  new_node->layer = layer;

  if(bitmap_file){
    new_node->bitmap = al_load_bitmap(bitmap_file);
    if(new_node->bitmap){
      new_node->bitmap_height = al_get_bitmap_height(new_node->bitmap);
      new_node->bitmap_width = al_get_bitmap_width(new_node->bitmap);
    }else{
      (void)puts("Bitmap file can't be found!");
    }
  }

  return new_node;
}

/* Bind a trigger to the specific node */
int bind_trigger(ENT_NODE *node, cpBool collision){
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
ENT_NODE *add_entity_text(cpVect position, char *string, unsigned char layer){
  ENT_NODE *new_node = NULL;

  if(string){
    new_node = calloc(1, sizeof(ENT_NODE));
    new_node->string = calloc(strlen(string), sizeof(char));
    memcpy(new_node->string, string, strlen(string));
    new_node->body_width = position.x;
    new_node->body_height = position.y;
    new_node->layer = layer;
    new_node->has_direct_pointer = cpFalse;
    DL_APPEND(ent_top, new_node);
  }
  return new_node;
}

/* Add a string object to our list of rendered stuff (Do not copy data from pointer) */
ENT_NODE *add_entity_text_direct(cpVect position, char *string, unsigned char layer){
  ENT_NODE *new_node = NULL;

  if(string){
    new_node = calloc(1, sizeof(ENT_NODE));
    new_node->string = string;
    new_node->body_width = position.x;
    new_node->body_height = position.y;
    new_node->layer = layer;
    new_node->has_direct_pointer = cpTrue;
    DL_APPEND(ent_top, new_node);
  }
  return new_node;
}

/* Add a node without any phtsical parameters */
ENT_NODE *add_entity_nophys(cpVect position, double width, double height, char *bitmap, unsigned char layer){
  ENT_NODE *new_node = NULL;

  if(bitmap){
    new_node = calloc(1, sizeof(ENT_NODE));
    new_node->bitmap = al_load_bitmap(bitmap);
    new_node->body_width = width;
    new_node->body_height = height;
    new_node->bitmap_height = al_get_bitmap_height(new_node->bitmap);
    new_node->bitmap_width = al_get_bitmap_width(new_node->bitmap);
    new_node->layer = layer;
    new_node->position = position;
    DL_APPEND(ent_top, new_node);
  }

  return new_node;
}

/* Add a progressbar entity */
ENT_NODE *add_entity_bar(cpVect position, double length, double height, double *monitored_value_pointer, unsigned char layer){
  ENT_NODE *new_node = NULL;

  if(monitored_value_pointer){
    new_node = calloc(1, sizeof(ENT_NODE));
    new_node->bitmap_height = height;
    new_node->bitmap_width = length;
    new_node->position = position;
    new_node->monitored_value_pointer = monitored_value_pointer;
    new_node->layer = layer;
    DL_APPEND(ent_top, new_node);
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
    al_destroy_bitmap(target->bitmap);
  }
}

/* Free all resources and shut down the engines */
void stop_interfacer(void){
  ENT_NODE *temp1 = NULL;
  DL_FOREACH(ent_top, temp1){
    DL_DELETE(ent_top, temp1);
    if(temp1->bitmap){
      al_destroy_bitmap(temp1->bitmap);
    }

    if(temp1->string == NULL && temp1->shape != NULL){
      cpSpaceRemoveShape(phys_space, temp1->shape);
      /* We did not add static bodies to space */
      if(cpBodyGetType(temp1->body) != CP_BODY_TYPE_STATIC){
        cpSpaceRemoveBody(phys_space, temp1->body);
        (void)puts("Removed body!");
      }
      cpBodyFree(temp1->body);
      cpShapeFree(temp1->shape);
    }else if(temp1->has_direct_pointer == cpFalse){
      (void)free(temp1->string);
    }
    (void)free(temp1);
  }
}
