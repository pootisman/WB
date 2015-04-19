#include "pre_menu.h"
#include "common_vars.h"
#include "player.h"
#include "game_phys.h"
#include "render.h"

/* TODO:
 * Think through error and warning reporting
 * should we use stderr or plain stdout?
 */

/* Display a simple greeting */
int show_greeting(void){
  ALLEGRO_BITMAP *background = al_load_bitmap("Background_space.jpeg");
  if(renderer.main_font){
    /* Wait for player to press enter or terminate the program on welcoming */
    while(1){
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_scaled_bitmap(background, 0, 0, 1600, 1200, 0, 0, renderer.view_width, renderer.view_height, 0);
      al_draw_text(renderer.main_font, al_map_rgb(241, 142, 0), 0.5f * renderer.view_width, 0.92f * renderer.view_height, ALLEGRO_ALIGN_CENTER, "Welcome, hit return to start.");
      al_flip_display();
      al_wait_for_event(renderer.main_queue, &renderer.event);

      al_get_keyboard_state(&renderer.kb_state);
      if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN && al_key_down(&renderer.kb_state, ALLEGRO_KEY_ENTER)){
        (void)puts("User accepted welcoming");
        al_destroy_bitmap(background);
        return EXIT_SUCCESS;
      }else if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        (void)puts("User decided to chicken out");
        al_destroy_bitmap(background);
        return EXIT_FAILURE;
      }
    }
  }
  al_destroy_bitmap(background);
  return EXIT_FAILURE;
}

/* Show info about a game */
int show_description(void){
  ALLEGRO_BITMAP *background = al_load_bitmap("Background_space.jpeg");
  if(renderer.main_font){
    /* Wait for player to finish reading info or terminate the program */
    while(1){
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_scaled_bitmap(background, 0, 0, 1600, 1200, 0, 0, renderer.view_width, renderer.view_height, 0);
      al_draw_text(renderer.main_font, al_map_rgb(241, 142, 0), 0.5f * renderer.view_width, 0.88f * renderer.view_height, ALLEGRO_ALIGN_CENTRE, "WizBall is a arcade scrolling game,");
      al_draw_text(renderer.main_font, al_map_rgb(241, 142, 0), 0.5f * renderer.view_width, 0.92f * renderer.view_height, ALLEGRO_ALIGN_CENTRE, "your task is to navigate around and shoot baddies.");
      al_flip_display();
      al_wait_for_event(renderer.main_queue, &renderer.event);
      al_get_keyboard_state(&renderer.kb_state);
      if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN && al_key_down(&renderer.kb_state, ALLEGRO_KEY_ENTER)){
        (void)puts("User read short info");
        al_destroy_bitmap(background);
        return EXIT_SUCCESS;
      }else if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        (void)puts("User decided to chicken out");
        al_destroy_bitmap(background);
        return EXIT_FAILURE;
      }
    }
  }
  al_destroy_bitmap(background);
  return EXIT_FAILURE;
}

/* Request name from user and store it for later use */
int request_name(void){
  ALLEGRO_BITMAP *background = al_load_bitmap("Background_space.jpeg");
  if(renderer.main_font){
    /* Wait for player to finish typing in his/her username */
    while(1){
      al_clear_to_color(al_map_rgb(0, 0, 0));
      al_draw_scaled_bitmap(background, 0, 0, 1600, 1200, 0, 0, renderer.view_width, renderer.view_height, 0);
      al_draw_text(renderer.main_font, al_map_rgb(241,142,0), 0.5f * renderer.view_width, 0.88f * renderer.view_height, ALLEGRO_ALIGN_CENTRE, "Please enter the name of player");
      if(single_player.name_length > 0){
        al_draw_text(renderer.main_font, al_map_rgb(241,142,0), 0.5f * renderer.view_width, 0.92f * renderer.view_height, ALLEGRO_ALIGN_CENTRE, &(single_player.player_name[0]));
      }
      al_flip_display();

      al_wait_for_event(renderer.main_queue, &renderer.event);

      al_get_keyboard_state(&renderer.kb_state);
      if(renderer.event.type == ALLEGRO_EVENT_KEY_DOWN && al_key_down(&renderer.kb_state, ALLEGRO_KEY_ENTER)){
        (void)printf("Name input complete. [%s]\n", single_player.player_name);
        al_destroy_bitmap(background);
        return EXIT_SUCCESS;
      }else if(renderer.event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        (void)puts("User decided to chicken out");
        al_destroy_bitmap(background);
        return EXIT_FAILURE;
      }else if(renderer.event.type == ALLEGRO_EVENT_KEY_CHAR){
        if(isprint(renderer.event.keyboard.unichar) && single_player.name_length < MAX_NAME_LEN){
          single_player.player_name[single_player.name_length] = renderer.event.keyboard.unichar;
          single_player.name_length++;
        }else if(renderer.event.keyboard.keycode == ALLEGRO_KEY_BACKSPACE && single_player.name_length > 0){
          single_player.player_name[single_player.name_length - 1] = 0;
          single_player.name_length--;
        }
      }
    }
  }
  al_destroy_bitmap(background);
  return EXIT_FAILURE;
}
