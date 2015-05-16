#ifndef _INTERFACER_
#define _INTERFACER_

#include <allegro5/allegro.h>
#include <chipmunk/chipmunk.h>

/* This is our new storage for bitmaps, old one was too bulky
 * now it is pretty slim */
typedef struct BITMAP_PLAIN{
  ALLEGRO_BITMAP *bitmap;
  struct BITMAP_PLAIN *prev, *next;
}BITMAP_PLAIN;

/* Moving physical body */
typedef struct ENT_PHYS_DYNAMIC{
  cpShape *shape;
  cpBody *body;
  double body_width, body_height;
  ALLEGRO_BITMAP *bitmap;
  unsigned char layer;
  struct ENT_PHYS_DYNAMIC *prev, *next;
}ENT_PHYS_DYNAMIC;

/* Stationary body */
typedef struct ENT_PHYS_STATIC{
  cpBody *body;
  cpShape *shape;
  double body_width, body_height;
  ALLEGRO_BITMAP *bitmap;
  unsigned char layer;
  struct ENT_PHYS_STATIC *prev, *next;
}ENT_PHYS_STATIC;

/* No collisions for this moving body */
typedef struct ENT_NOPHYS_DYNAMIC{
  double position_x, position_y;
  double size_x, size_y;
  ALLEGRO_BITMAP *bitmap;
  unsigned char layer;
  struct ENT_NOPHYS_DYNAMIC *prev, *next;
}ENT_NOPHYS_DYNAMIC;

/* No collision for this static body */
typedef struct ENT_NOPHYS_STATIC{
  double position_x, position_y;
  double size_x, size_y;
  ALLEGRO_BITMAP *bitmap;
  unsigned char layer;
  struct ENT_NOPHYS_STATIC *prev, *next;
}ENT_NOPHYS_STATIC;

/* Text */
typedef struct ENT_NOPHYS_TEXT{
  double position_x, position_y;
  ALLEGRO_COLOR text_color;
  unsigned char is_a_copy, layer;
  char *string;
  struct ENT_NOPHYS_TEXT *prev, *next;
}ENT_NOPHYS_TEXT;

/* Progress bar */
typedef struct ENT_NOPHYS_PROGBAR{
  double position_x, position_y;
  double length, height;
  double *monitored_value;
  ALLEGRO_COLOR progressbar_color;
  ALLEGRO_COLOR progresstxt_color;
  unsigned char render_mode, layer;
  struct ENT_NOPHYS_PROGBAR *prev, *next;
}ENT_NOPHYS_PROGBAR;

/* Simple line */
typedef struct ENT_NOPHYS_LINE{
  cpVect p1, p2;
  ALLEGRO_COLOR color;
  struct ENT_NOPHYS_LINE *prev, *next;
  unsigned char layer;
}ENT_NOPHYS_LINE;

/* Trigger with physical collisions (frying pan) */
typedef struct ENT_PHYS_TRIGGER{
  cpShape *shape;
  cpBody *body;
  double position_x, position_y;
  ALLEGRO_BITMAP *bitmap;
  unsigned char layer;
  struct ENT_PHYS_TRIGGER *prev, *next;
}ENT_PHYS_TRIGGER;

extern BITMAP_PLAIN *plaint_bitmap_list;
extern unsigned int bitmap_counter;

extern ENT_PHYS_DYNAMIC *dynamic_phys_body_list;
extern unsigned int dynamic_phys_body_count;

extern ENT_PHYS_STATIC *static_phys_body_list;
extern unsigned int static_phys_body_count;

extern ENT_NOPHYS_DYNAMIC *dynamic_nophys_body_list;
extern unsigned int dynamic_nophys_body_count;

extern ENT_NOPHYS_STATIC *static_nophys_body_list;
extern unsigned int static_nophys_body_count;

extern ENT_NOPHYS_TEXT *nophys_text_list;
extern unsigned int nophys_text_count;

extern ENT_NOPHYS_PROGBAR *nophys_progress_list;
extern unsigned int nophys_progress_count;

extern ENT_PHYS_TRIGGER *phys_trigger_list;
extern unsigned int phys_trigger_count;

extern ENT_NOPHYS_LINE *nophys_line_list;
extern unsigned int nophys_line_count;

ENT_PHYS_DYNAMIC *add_entity_mobile(cpVect position, double radius, double mass, double angle, unsigned int bitmap, unsigned char layer);
ENT_PHYS_STATIC *add_entity_static(cpVect position, double width, double height, double angle, unsigned int bitmap, unsigned char layer);
ENT_NOPHYS_TEXT *add_entity_text(cpVect position, char *string, unsigned char layer);
ENT_NOPHYS_TEXT *add_entity_text_direct(cpVect position, char *string, unsigned char layer);
ENT_NOPHYS_STATIC *add_entity_nophys(cpVect position, double width, double height, unsigned int bitmap, unsigned char layer);
ENT_NOPHYS_PROGBAR *add_entity_bar(cpVect position, double length, double height, double *monitored_value, unsigned char layer);
ENT_NOPHYS_LINE *add_entity_line(cpVect point1, cpVect point2, unsigned char layer);
int bind_trigger(ENT_PHYS_STATIC *node, cpBool collision);
int bind_dead(ENT_PHYS_STATIC *node);
int bind_level_seam(ENT_PHYS_STATIC *node);
int bind_bomb_trigger(ENT_PHYS_DYNAMIC *node);
int bind_bomb_kaboom(ENT_PHYS_DYNAMIC *node);
int bind_powerup(ENT_PHYS_DYNAMIC *node);
int bind_laser(ENT_PHYS_DYNAMIC *node);
int bind_range(ENT_PHYS_DYNAMIC *node);
int bind_spPwup(ENT_PHYS_DYNAMIC *node);

void remove_ent_phy_dyn(ENT_PHYS_DYNAMIC *target);

void free_entities(void);

void stop_interfacer(void);

#endif
