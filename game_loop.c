#include <stdio.h>
#include <sys/time.h>
#include "interscreen.h"
#include "interfacer.h"
#include "player.h"
#include "bomb.h"
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
char level_string[16] = {0};
char shield_string[16] = {0};
char score_string[16] = {0};
char spPwup_string[16] = {0};

unsigned int level_counter = 0;

float display_x_offset = 0.0f;
cpBool did_reach_teleport = cpFalse;

inline void add_platform(cpVect position, double rotation){
  add_entity_static(position, DEF_PLAT_WIDTH, DEF_PLAT_HEIGHT, rotation, 2, RENDER_NUM_LAYERS - 2);
}

inline void add_hole(cpVect position){
  ENT_PHYS_STATIC *temp = add_entity_static(position, DEF_PLAT_WIDTH, DEF_PLAT_HEIGHT, 0.0, 0,  RENDER_NUM_LAYERS - 2);
  bind_trigger(temp, cpFalse);
}

inline void add_deathwall(cpVect position){
  ENT_PHYS_STATIC *temp = add_entity_static(position, DEF_PLAT_WIDTH, DEF_PLAT_HEIGHT, 0.0, 0, RENDER_NUM_LAYERS - 2);
  bind_dead(temp);
}

inline void add_level_teleport(cpVect position){
  ENT_PHYS_STATIC *temp = add_entity_static(position, 500, renderer.view_height, 0.0, 0,  RENDER_NUM_LAYERS - 2);
  bind_level_seam(temp);
}

/* Load critical resources, such as background image,
 * platform textures and player ball.
 */
void init_critical(){
  display_x_offset = 0.0;
  precache_bitmap("Background_space.tga");
  precache_bitmap("Platform.tga");
  precache_bitmap("Ball.tga");
  precache_bitmap("Bomb.tga");
  precache_bitmap("PWUp.tga");
  precache_bitmap("spPwup.tga");

}

/* Player, background, GUI and the whole level are initialized here */
void init_level(){
  ENT_PHYS_DYNAMIC *temp_dyn;
  ENT_PHYS_STATIC *temp_static;
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
  add_entity_bar(cpv(250, renderer.view_height - 15), renderer.view_width * 0.5, 10, &timeleft, RENDER_NUM_LAYERS - 1);

  /* Kludge master */
  temp_dyn = add_entity_mobile(cpv(40.0, 100.0), DEF_PLAYER_RADIUS, DEF_PLAYER_MASS, 0.0, 3, RENDER_NUM_LAYERS - 2);
  spawn_player(temp_dyn->body);

  /* Create a teleport to next level */
  temp_static = add_entity_static(cpv(renderer.view_width * 5, 0), renderer.view_width, renderer.view_height, 0.0, 0, RENDER_NUM_LAYERS - 2);
  bind_level_seam(temp_static);

  sprintf(&(playing_string[0]), "Player %s is in the game.", single_player.player_name); 
  add_entity_text_direct( cpv(10, 10), &(playing_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 30), &(health_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 50), &(level_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 70), &(shield_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 90), &(score_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 110), &(spPwup_string[0]), RENDER_NUM_LAYERS - 1);
}

/* Generate a chunk of our level */
void gen_chunk(){
  int i;

  for(i = -150; i < 150; i++){
    if((double)rand()/(double)RAND_MAX > 0.5){
      add_platform(cpv(32+64*i, renderer.view_height - 16), 0.0);
      if((double)rand()/(double)RAND_MAX > 0.9/level_counter){
        spawn_bomb(cpv(32+64*i,renderer.view_height - 48));
      }else if((double)rand()/(double)RAND_MAX > 0.5/level_counter){
        bind_powerup(add_entity_mobile(cpv(32+64*i,renderer.view_height - 48), BOMB_RADIUS, 1.0, 0.0, 5, RENDER_NUM_LAYERS - 2));
      }else if((double)rand()/(double)RAND_MAX > 0.4/level_counter){
        bind_spPwup(add_entity_mobile(cpv(32+64*i,renderer.view_height - 256), BOMB_RADIUS, 1.0, 0.0, 6, RENDER_NUM_LAYERS - 2));
      }
    }else{
      add_hole(cpv(32+ 64*i, renderer.view_height - 16));
      add_deathwall(cpv(32+ 64*i, renderer.view_height + 16));
    }
    if((double)rand()/(double)RAND_MAX > 0.3){
      add_platform(cpv(32+64*i, 16), 0.0);
    }else{
      add_hole(cpv(32+ 64*i, 16));
      add_deathwall(cpv(32 + 64*i, -16));
    }

    if(rand() % 5 == 0){
      add_platform(cpv(32 + 64 * i, 48 + (renderer.view_height - 96) * ((int)(((double)rand()/(double)RAND_MAX)*10))/10.0), 0.0);
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

  /* Apply the transform to each level with different factors */
  al_identity_transform(&trans);
  al_translate_transform(&trans, -display_x_offset, 0.0);
  apply_trans_to_layer(&trans, RENDER_NUM_LAYERS - 2);

  al_identity_transform(&trans);
  al_translate_transform(&trans, -display_x_offset * 0.1, 0.0);
  apply_trans_to_layer(&trans, 0);
}

void draw_until_escape(void){
  while(1){
    render_layers();
    render_finalize_and_draw();
    al_wait_for_event(renderer.main_queue, &renderer.event);
    al_get_keyboard_state(&renderer.kb_state);
    if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN && al_key_down(&renderer.kb_state, ALLEGRO_KEY_ESCAPE)){
      return EXIT_SUCCESS;
    }else if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      return EXIT_FAILURE;
    }

  }
}

/* Loop functioon for our game, UGLY GOTO DETECTED! */
int run_loop(){

  gamereset:
  level_counter++;
  did_reach_teleport = cpFalse;
  init_critical();
  init_level();

  gettimeofday(&time_now, NULL);
  time_stop.tv_sec = time_now.tv_sec + TIMEOUT;
  sprintf(&(level_string[0]), "Level %d", level_counter);

  while(single_player.health > 0 && time_stop.tv_sec > time_now.tv_sec){
    gettimeofday(&time_now, NULL);
    sprintf(&(shield_string[0]), "Shields %d", single_player.buffed);
    sprintf(&(score_string[0]), "Score %d", single_player.score);
    sprintf(&(spPwup_string[0]), "Power %d", single_player.spPwup);

    timeleft = (double)(time_stop.tv_sec - time_now.tv_sec)/(double)TIMEOUT;

    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_event_until(renderer.main_queue, &renderer.event, &renderer.timeout);

    if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      (void)puts("User decided to quit the game.");
      break;
    }

    al_get_keyboard_state(&renderer.kb_state);
    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_DOWN)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, 150.0), cpv(0.0, 0.0));
      (void)puts("Down key pressed, applying impulse.");
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_UP)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, -150.0), cpv(0.0, 0.0));
      (void)puts("Up key pressed, applying impulse.");
    }

    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_LEFT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(-150.0, 0.0), cpv(0.0, 0.0));
      (void)puts("Left key pressed, applying impulse.");
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_RIGHT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(150.0, 0.0), cpv(0.0, 0.0));
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

    free_entities();

    cpSpaceStep(phys_space, DEF_TIME_STEP);

    if(did_reach_teleport == cpTrue && single_player.spPwup >= 100){
      stop_interfacer();
      goto gamereset;
    }
  }

  prepare_scores(single_player.score, single_player.player_name);
  draw_until_escape();


  return EXIT_SUCCESS;
}

/* Do this before return EXIT_SUCCESS in main */
void stop_game(){
  stop_interfacer();
  stop_phys();
  stop_render();
}
