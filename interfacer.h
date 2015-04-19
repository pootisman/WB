#ifndef _INTERFACER_
#define _INTERFACER_

#include <allegro5/allegro.h>
#include <chipmunk/chipmunk.h>

/* Generic entity node */
typedef struct ENT_NODE{
  /* Shape, body and bitmap are NULL if we want to render a string */
  cpShape *shape;
  cpBody *body;
  cpBool has_direct_pointer;
  cpVect position;
  /* Bitmap is null if the object is a trigger */
  ALLEGRO_BITMAP *bitmap;
  char *string;
  unsigned short bitmap_width, bitmap_height;
  /* !WARNING!
   * These are used as coordinates in case of string */
  double body_width, body_height;
  unsigned char layer;
  /* Used for progressbars */
  double *monitored_value_pointer;
  struct ENT_NODE *next, *prev;
}ENT_NODE;

typedef struct BITMAP_PLAIN{
  ALLEGRO_BITMAP *bitmap;
  struct BITMAP_PLAIN *prev, *next;
}BITMAP_PLAIN;

typedef struct ENT_PHYS_DYNAMIC_BMAP{
  cpShape *shape;
  cpBody *body;
  ALLEGRO_BITMAP *bitmap;
}ENT_DYNAMIC_BMAP;

typedef struct ENT_PHYS_STATIC_BMAP{
  cpShape *shape;
  ALLEGRO_BITMAP *bitmap;
}ENT_STATIC_BMAP;

typedef struct ENT_NOPHYS_BMAP{
  double position_x, position_y;
  double size_x, size_y;
  ALLEGRO_BITMAP *bitmap;
}ENT_NOPHYS_DYNAMIC_BMAP;

typedef struct ENT_NOPHYS_TEXT{
  double position_x, position_y;
  ALLEGRO_COLOR text_color;
  unsigned char is_a_copy;
  char *text;
}ENT_NOPHYS_TEXT;

typedef struct ENT_NOPHYS_PROGBAR{
  double position_x, position_y;
  double length, height;
  ALLEGRO_COLOR progressbar_color;
  ALLEGRO_COLOR progresstxt_color;
  unsigned char render_mode;
}ENT_NOPHYS_PROGBAR;

typedef struct ENT_NOPHYS_TRIGGER{
  cpShape *shape;
  cpBody *body;
  double position_x, position_y;
}ENT_NOPHYS_TRIGGER;

typedef struct ENT_NOPHYS_TRIGGER_BMAP{
  cpShape *shape;
  cpBody *body;
  double position_x, position_y;
  ALLEGRO_BITMAP *bitmap;
}ENT_NOPHYS_TRIGGER_BMAP;

extern BITMAP_PLAIN *plaint_bitmap_list;
extern unsigned int bitmap_counter;

extern ENT_DYNAMIC_BMAP *ent_bmap_top;
extern ENT_DYNAMIC_BMAP *ent_bmap_top;
extern ENT_DYNAMIC_BMAP *ent_bmap_top;
extern ENT_DYNAMIC_BMAP *ent_bmap_top;

extern ENT_NODE *ent_top;

ENT_NODE *add_entity_mobile(cpVect position, double radius, double mass, unsigned int bitmap, unsigned char layer);
ENT_NODE *add_entity_static(cpVect position, double width, double height, unsigned int bitmap, unsigned char layer);
ENT_NODE *add_entity_text(cpVect position, char *string, unsigned char layer);
ENT_NODE *add_entity_text_direct(cpVect position, char *string, unsigned char layer);
ENT_NODE *add_entity_nophys(cpVect position, double width, double height, unsigned int bitmap, unsigned char layer);
ENT_NODE *add_entity_bar(cpVect position, double length, double height, double *monitored_value, unsigned char layer);
int bind_trigger(ENT_NODE *node, cpBool collision);
void render_node(ENT_NODE *node);
void del_entity(ENT_NODE *target);
void stop_interfacer(void);

#endif
