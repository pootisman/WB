#include <stdio.h>
#include <sys/time.h>
#include "interfacer.h"
#include "player.h"
#include "game_phys.h"
#include "game_loop.h"
#include "render.h"
#include "utlist.h"

/* Default platform parameters */
#define DEF_PLAT_HEIGHT 32
#define DEF_PLAT_WIDTH 64

/* Parameters of the display zones for scrolling */
#define TRACKING_THRESHOLD 0.6
#define TRACKING_MULTIPLIER 1.5
#define TRACKING_BASE 8

/* Time per level */
#define TIMEOUT 120 /* seconds */

struct timeval time_now, time_stop;
double timeleft = 0.0;
char playing_string[128] = {0};
char health_string[32] = {0};

float display_x_offset = 0.0f;

inline void add_platform(cpVect position){
  add_entity_static(position, DEF_PLAT_WIDTH, DEF_PLAT_HEIGHT, 2, RENDER_NUM_LAYERS - 2);
}

inline void add_hole(cpVect position){
  ENT_PHYS_STATIC *temp = add_entity_static(position, DEF_PLAT_WIDTH, DEF_PLAT_HEIGHT, 0,  RENDER_NUM_LAYERS - 2);
  bind_trigger(temp, cpFalse);
}

/* Load critical resources, such as background image,
 * platform textures and player ball.
 */
void init_critical(){
  precache_bitmap("Background_space.jpeg");
  precache_bitmap("Platform.png");
  precache_bitmap("Ball.png");
}

/* TODO: Fix double spawn of the player somehow */

/* Player is initialized here */
void init_level(){
  ENT_PHYS_DYNAMIC *temp;
  /* Add ground */
  gen_chunk();
  /* Add background
     NOTE TO SELF: Kludge powers activate! */
  add_entity_nophys(cpv(renderer.view_width/2.0 + 2.0*renderer.view_width, 0), renderer.view_width + renderer.view_width * 0.1, renderer.view_height + renderer.view_height * 0.1, 1, 0);
  add_entity_nophys(cpv(-renderer.view_width/2.0 - renderer.view_width, 0), renderer.view_width + renderer.view_width * 0.1, renderer.view_height + renderer.view_height * 0.1, 1, 0);

  add_entity_nophys(cpv(renderer.view_width/2.0, 0), renderer.view_width + renderer.view_width * 0.1, renderer.view_height + renderer.view_height * 0.1, 1, 0);
  add_entity_nophys(cpv(renderer.view_width/2.0 + renderer.view_width, 0), renderer.view_width + renderer.view_width * 0.1, renderer.view_height + renderer.view_height * 0.1, 1, 0);

  add_entity_nophys(cpv(-renderer.view_width/2.0, 0), renderer.view_width + renderer.view_width * 0.1, renderer.view_height + renderer.view_height * 0.1, 1, 0);
  /* Add timer and a bar */
  add_entity_bar(cpv(250, 15), renderer.view_width * 0.5, 10, &timeleft, RENDER_NUM_LAYERS - 1);

  /* Kludge master */
  spawn(NULL);
  temp = add_entity_mobile(cpv(40.0, 100.0), single_player.radius, single_player.mass, 3, RENDER_NUM_LAYERS -2);
  spawn(temp->body);



  sprintf(&(playing_string[0]), "Player %s is in the game.", single_player.player_name);
  add_entity_text_direct( cpv(10, 10), &(playing_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 30), &(health_string[0]), RENDER_NUM_LAYERS - 1);
}

/* Generate a next chunk of our level */
void gen_chunk(){
  int i;

  for(i = -150; i < 150; i++){
    if((double)rand()/(double)RAND_MAX > 0.5){
      add_platform(cpv(32+64*i, renderer.view_height - 16));
    }else{
      add_hole(cpv(32+ 64*i, renderer.view_height - 16));
    }
    if((double)rand()/(double)RAND_MAX > 0.3){
      add_platform(cpv(32+64*i, 16));
    }else{
      add_hole(cpv(32+ 64*i, 16));
    }

    if(rand() % 5 == 0){
      add_platform(cpv(32 + 64 * i, 48 + (renderer.view_height - 96) * ((int)(((double)rand()/(double)RAND_MAX)*10))/10.0));
    }

  }
}

/* Keep generating new transformation in order to keep our player on screen */
void track_player(){
  cpVect player_position = cpBodyGetPosition(single_player.body);
  ALLEGRO_TRANSFORM trans;

  /* If player is within a certain rectangle, do not change any transformations */
  if(player_position.x >= (TRACKING_THRESHOLD * renderer.view_width + display_x_offset) ||
     player_position.x <= ((1.0 - TRACKING_THRESHOLD) * renderer.view_width + display_x_offset)){
    /* Now we have to recompute slide speed and coordinates */
    if(player_position.x <= TRACKING_THRESHOLD * renderer.view_width + display_x_offset){
      display_x_offset -= pow(TRACKING_BASE, fabs((player_position.x - renderer.view_width/2.0 - display_x_offset)/((1.0 - TRACKING_THRESHOLD) * renderer.view_width)) * TRACKING_MULTIPLIER);
      (void)printf("Decreasing horisontal offset to %f\n", display_x_offset);
    }else{
      display_x_offset += pow(TRACKING_BASE, fabs((player_position.x - renderer.view_width/2.0 - display_x_offset)/((1.0 - TRACKING_THRESHOLD) * renderer.view_width)) * TRACKING_MULTIPLIER);
      (void)printf("Increasing horisontal offset to %f\n", display_x_offset);
    }
  }

  al_identity_transform(&trans);
  al_translate_transform(&trans, -display_x_offset, 0.0);
  apply_trans_to_layer(&trans, RENDER_NUM_LAYERS - 2);

  al_identity_transform(&trans);
  al_translate_transform(&trans, -display_x_offset * 0.1, 0.0);
  apply_trans_to_layer(&trans, 0);
}

/* Loop functioon for our game */
int run_loop(){
  gettimeofday(&time_now, NULL);
  time_stop.tv_sec = time_now.tv_sec + TIMEOUT;

  while(single_player.health > 0 && time_stop.tv_sec > time_now.tv_sec){
    gettimeofday(&time_now, NULL);

    timeleft = (double)(time_stop.tv_sec - time_now.tv_sec)/(double)TIMEOUT;

    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_event_until(renderer.main_queue, &renderer.event, &renderer.timeout);

    if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      (void)puts("User decided to quit the game.");
      break;
    }

    al_get_keyboard_state(&renderer.kb_state);
    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_DOWN)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, 120.0), cpv(0.0, 0.0));
      (void)puts("Down key pressed, applying impulse.");
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_UP)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, -120.0), cpv(0.0, 0.0));
      (void)puts("Up key pressed, applying impulse.");
    }

    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_LEFT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(-120.0, 0.0), cpv(0.0, 0.0));
      (void)puts("Left key pressed, applying impulse.");
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_RIGHT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(120.0, 0.0), cpv(0.0, 0.0));
      (void)puts("Right key pressed, applying impulse.");
    }

    if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN){
      switch(renderer.event.keyboard.keycode){
        case(ALLEGRO_KEY_W):{
          gravity.y = -GRAVITY;
          gravity.x = 0.0;
          cpSpaceSetGravity(phys_space, gravity);
          break;
        }
        case(ALLEGRO_KEY_S):{
          gravity.y = GRAVITY;
          gravity.x = 0.0;
          cpSpaceSetGravity(phys_space, gravity);
          break;
        }
        case(ALLEGRO_KEY_A):{
          gravity.y = 0.0;
          gravity.x = -GRAVITY;
          cpSpaceSetGravity(phys_space, gravity);
          break;
        }
        case(ALLEGRO_KEY_D):{
          gravity.y = 0.0;
          gravity.x = GRAVITY;
          cpSpaceSetGravity(phys_space, gravity);
          break;
        }
      }
    }

    track_player();
    (void)sprintf(&(health_string[0]), "Health: %d", single_player.health);

    render_layers();
    render_finalize_and_draw();

    cpSpaceStep(phys_space, DEF_TIME_STEP);
  }

  return EXIT_SUCCESS;
}

/* Do this before return EXIT_SUCCESS in main */
void stop_game(){
  stop_interfacer();
  stop_phys();
  stop_render();
}
