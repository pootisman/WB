#ifndef _RENDER_
#define _RENDER_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "interfacer.h"

#define RENDER_NUM_LAYERS 8

typedef struct RENDER{
  ALLEGRO_DISPLAY *display;
  ALLEGRO_FONT *main_font;
  ALLEGRO_EVENT_QUEUE *main_queue;
  ALLEGRO_EVENT_SOURCE *keyboard_source, *display_source;
  ALLEGRO_EVENT event;
  ALLEGRO_TIMEOUT timeout;
  ALLEGRO_KEYBOARD_STATE kb_state;
  ALLEGRO_BITMAP *layers[RENDER_NUM_LAYERS];
  ALLEGRO_TRANSFORM layer_transforms[RENDER_NUM_LAYERS];
  unsigned short view_width, view_height;
}RENDER;

extern RENDER renderer;

int init_render(unsigned short view_width, unsigned short view_height);

void add_trans_to_layer(ALLEGRO_TRANSFORM *transform, unsigned char target_layer);
void apply_trans_to_layer(ALLEGRO_TRANSFORM *transform, unsigned char target_layer);
void render_to_layer(ENT_NODE *object, unsigned char target_layer);
void render_finalize_and_draw(void);

void stop_render(void);

#endif
