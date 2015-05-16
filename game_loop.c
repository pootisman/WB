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
#define LEVEL_LENGTH_MULTIPLIER 2

/* Time per level */
#define TIMEOUT 120 /* seconds */

struct timeval time_now, time_stop;
double timeleft = 0.0, chargeleft = 1.0;
char playing_string[128] = {0};
char health_string[32] = {0};
char level_string[16] = {0};
char shield_string[16] = {0};
char score_string[16] = {0};
char laser_string[24] = {0};
unsigned int number_of_lives = 1;
unsigned int score_lives = 0;
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
  precache_bitmap("Key.tga");
  precache_bitmap("Teleport.tga");
  precache_bitmap("Vertical.tga");
  precache_bitmap("Laser.tga");
}

/*
 * Spawn isolation platforms of the level.
 */
void isolate_level(cpVect portal_position){
  int i = 0, j = 0;
  ENT_PHYS_STATIC *temp_static = NULL;

  for(i = 32; i < renderer.view_height+32; i+=64){
    if(i < (portal_position.y - 64) || i > (portal_position.y + 64)){
      add_entity_static(cpv(portal_position.x, i), 64, 64, 0, 8, RENDER_NUM_LAYERS - 2);
    }else{
      /* Create a teleport to next level */
      add_entity_static(cpv(portal_position.x+64, i), 64, 64, 0, 8, RENDER_NUM_LAYERS - 2);
      add_entity_static(cpv(portal_position.x+64, i+64), 64, 64, 0, 8, RENDER_NUM_LAYERS - 2);
      temp_static = add_entity_static(cpv(portal_position.x, i + 32), 64, 128, 0.0, 7, RENDER_NUM_LAYERS - 2);
      i+=64;
      bind_level_seam(temp_static);
    }
  }

  for(i = 32; i < renderer.view_height; i+=64){
    add_entity_static(cpv(-portal_position.x + 32, i), 64, 64, 0, 8, RENDER_NUM_LAYERS - 2);
  }
}

/* Generate a chunk of our level
 * Inlined since we use it only here.
 */
inline void gen_chunk(int start, int end){
  int i;

  for(i = start; i < end; i++){
    if((double)rand()/(double)RAND_MAX > 0.5){
      add_platform(cpv(32+64*i, renderer.view_height - 16), 0.0);
      if((double)rand()/(double)RAND_MAX > 0.99/(level_counter * 0.5)){
        spawn_bomb(cpv(32+64*i,renderer.view_height - 48));
      }else if((double)rand()/(double)RAND_MAX > 0.5/level_counter){
        bind_powerup(add_entity_mobile(cpv(32+64*i,renderer.view_height - 48), BOMB_RADIUS, 1.0, 0.0, 5, RENDER_NUM_LAYERS - 2));
      }else if((double)rand()/(double)RAND_MAX > 0.4/level_counter){
        bind_spPwup(add_entity_mobile(cpv(32+64*i,renderer.view_height - 256), BOMB_RADIUS, 1.0, 0.0, 6, RENDER_NUM_LAYERS - 2));
      }else if((double)rand()/(double)RAND_MAX > 0.4/level_counter){
        bind_laser(add_entity_mobile(cpv(32+64*i,renderer.view_height - 256), BOMB_RADIUS, 1.0, 0.0, 9, RENDER_NUM_LAYERS - 2));
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

/* Player, background, GUI and the whole level are initialized here */
void init_level(){
  ENT_PHYS_DYNAMIC *temp_dyn;
  cpConstraint *constraint;
  int i = 0;

  /* Add ground */
  gen_chunk(-(renderer.view_width/64)*level_counter * LEVEL_LENGTH_MULTIPLIER, (renderer.view_width/64)*level_counter * LEVEL_LENGTH_MULTIPLIER);
  /* Add background
     NOTE TO SELF: Kludge powers activate! */
  for(i = -level_counter * LEVEL_LENGTH_MULTIPLIER; i <= (int)level_counter * LEVEL_LENGTH_MULTIPLIER;add_entity_nophys(cpv(renderer.view_width/2.0 + i*renderer.view_width, 0), renderer.view_width, renderer.view_height, 1, 0), i++);

  /* Add timer and a bar */
  add_entity_bar(cpv(20, renderer.view_height - 15), renderer.view_width - 40, 10, &timeleft, RENDER_NUM_LAYERS - 1);
  add_entity_bar(cpv(20, renderer.view_height - 65), renderer.view_width - 40, 2, &chargeleft, RENDER_NUM_LAYERS - 1);

  /* Kludge master */
  temp_dyn = add_entity_mobile(cpv(renderer.view_width/2.0, renderer.view_height/2.0), DEF_PLAYER_RADIUS, DEF_PLAYER_MASS, 0.0, 3, RENDER_NUM_LAYERS - 2);
  spawn_player(temp_dyn->body);
  temp_dyn = add_entity_mobile(cpv(renderer.view_width/2.0, renderer.view_height/2.0), DEF_PLAYER_RADIUS*10, 1, 0.0, 0, 0);
  constraint = cpSpaceAddConstraint( phys_space, cpPinJointNew(single_player.body, temp_dyn->body, cpv(0.0, 0.0), cpv(0.0, 0.0)));
  bind_range(temp_dyn);
  single_player.weapon_range = temp_dyn;

  /* And isolate the later parts */
  isolate_level(cpv(renderer.view_width * level_counter * LEVEL_LENGTH_MULTIPLIER - 16, renderer.view_height/2.0));

  sprintf(&(playing_string[0]), "Player %s is in the game.", single_player.player_name); 
  add_entity_text_direct( cpv(10, 10), &(playing_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 30), &(health_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 50), &(level_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 70), &(shield_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 90), &(score_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 110), &(spPwup_string[0]), RENDER_NUM_LAYERS - 1);
  add_entity_text_direct( cpv(10, 130), &(laser_string[0]), RENDER_NUM_LAYERS - 1);
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
#ifdef DEBUG
      (void)printf("Decreasing horisontal offset to %f\n", display_x_offset);
#endif
    }else{
      display_x_offset += pow(TRACKING_BASE, fabs((player_position.x - renderer.view_width/2.0 - display_x_offset)/((1.0 - TRACKING_THRESHOLD) * renderer.view_width)) * TRACKING_MULTIPLIER);
#ifdef DEBUG
      (void)printf("Increasing horisontal offset to %f\n", display_x_offset);
#endif
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

/* Redraw the screen until given key is pressed
   Inline since we use it only here and do not
   need pointers of it */
inline void draw_until_key(int switch_key){
  while(1){
    render_layers();
    render_finalize_and_draw();
    al_wait_for_event(renderer.main_queue, &renderer.event);
    al_get_keyboard_state(&renderer.kb_state);
    if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN && al_key_down(&renderer.kb_state, switch_key)){
      return;
    }else if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      return;
    }

  }
}

/* Loop functioon for our game, UGLY GOTO DETECTED!
   The programmer will burn in HELL!!! */
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
    score_lives = single_player.score;
    if(score_lives != 0){
      number_of_lives += score_lives / DEF_PLAYER_BONUS_STEP;
      score_lives %= DEF_PLAYER_BONUS_STEP;
    }
    /* Initialize all needed variables, strings for display */
    gettimeofday(&time_now, NULL);

    sprintf(&(shield_string[0]), "Shields %d", single_player.buffed);
    sprintf(&(score_string[0]), "Score %d", single_player.score);
    sprintf(&(spPwup_string[0]), "Power %d", single_player.spPwup);
    sprintf(&(laser_string[0]), "Laser %s", single_player.laser ? ("Enabled") : ("Disabled"));
    sprintf(&(laser_string[0]), "White hole %s", single_player.laser ? ("Enabled") : ("Disabled"));

    timeleft = (double)(time_stop.tv_sec - time_now.tv_sec)/(double)TIMEOUT;
    chargeleft = (double)single_player.charge/(double)UCHAR_MAX;
    al_clear_to_color(al_map_rgb(0,0,0));
    al_wait_for_event_until(renderer.main_queue, &renderer.event, &renderer.timeout);

    if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
#ifdef DEBUG
      (void)puts("User decided to quit the game.");
#endif
      break;
    }

    /* This section is resposible for vertical thrusters */
    al_get_keyboard_state(&renderer.kb_state);
    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_DOWN)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, 150.0), cpv(0.0, 0.0));
#ifdef DEBUG
      (void)puts("Down key pressed, applying impulse.");
#endif
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_UP)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(0.0, -150.0), cpv(0.0, 0.0));
#ifdef DEBUG
      (void)puts("Up key pressed, applying impulse.");
#endif
    }

    /* This one controls steering thrusters */
    if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_LEFT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(-150.0, 0.0), cpv(0.0, 0.0));
#ifdef DEBUG
      (void)puts("Left key pressed, applying impulse.");
#endif
    }else if(al_key_down(&renderer.kb_state, ALLEGRO_KEY_RIGHT)){
      cpBodyApplyImpulseAtWorldPoint(single_player.body, cpv(150.0, 0.0), cpv(0.0, 0.0));
#ifdef DEBUG
      (void)puts("Right key pressed, applying impulse.");
#endif
    }

    /* Here we can switch gravity with WASD buttons */
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
        case(ALLEGRO_KEY_1):{
          if(single_player.has_laser){
            single_player.laser = !single_player.laser;
          }
          break;
        }
        case(ALLEGRO_KEY_2):{
          if(single_player.has_white_hole){
            single_player.grav = !single_player.grav;
          }
          break;
        }
      }
    }

    if(single_player.charge < UCHAR_MAX){
      single_player.charge++;
    }

    track_player();
    (void)sprintf(&(health_string[0]), "Health: %d", single_player.health);

    render_layers();
    render_finalize_and_draw();

    /* Delete entities that are no longer used */
    free_entities();

    cpSpaceStep(phys_space, DEF_TIME_STEP);

    if(did_reach_teleport == cpTrue){
      stop_interfacer();
      goto gamereset;
    }
  }

  --number_of_lives;
  if(number_of_lives > 0){
    single_player.health = 255;
    stop_interfacer();
    goto gamereset;
  }
  /* Loop is over, show scores and save new score (or don't) */
  prepare_scores(single_player.score, single_player.player_name);
  draw_until_key(ALLEGRO_KEY_ESCAPE);


  return EXIT_SUCCESS;
}

/* Do this before return EXIT_SUCCESS in main */
void stop_game(){
  stop_interfacer();
  stop_phys();
  stop_render();
}
