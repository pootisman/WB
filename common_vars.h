#ifndef _COMMON_VARS_
#define _COMMON_VARS_

#define MAX_NAME_LEN 64 /* Max length of player name */
#define PHYS_STEP_SZ 0.01 /* 10 ms step size for physics solver */

/* Needed for displaying game */
#ifdef NEED_ALLEGRO
/* Info for displaying */
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_BITMAP  *background, *player_sprite;
extern ALLEGRO_FONT *main_font;
extern ALLEGRO_EVENT_QUEUE *main_queue;
extern ALLEGRO_EVENT_SOURCE *keyboard_source, *display_source;
extern ALLEGRO_EVENT event;
extern ALLEGRO_TIMEOUT timeout;
extern ALLEGRO_KEYBOARD_STATE kb_state;

extern unsigned short x_size, y_size;
#endif

/* Used by physics solver */
#ifdef NEED_PHYS
/* Physics data, needed always */
extern cpSpace *phys_space;
extern cpVect gravity;
extern cpShape *bed;
extern cpShape *player_shape;
extern cpFloat phys_time_now;
extern cpFloat player_mass, player_radius, player_moment;
extern cpBody *player_body;
#endif

#endif
