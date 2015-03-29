#ifndef _PRE_MENU_
#define _PRE_MENU_

#include <stdio.h>
#include <ctype.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

int init_needed(unsigned short x_res, unsigned short y_res);

int show_greeting(void);
int show_description(void);
int request_name(void);

void stop_graph(void);

#endif
