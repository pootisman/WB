#include <stdio.h>
#include "render.h"
#include "utlist.h"

RENDER renderer;

/* Initialize base, image, text, fonts */
int init_render(unsigned short view_width, unsigned short view_height){
  unsigned char i = 0;

  /* Initialize Allegro engine */
  (void)puts("Initializing Allegro engine");

  (void)fputs("[al_init>--", stdout);
  if(!al_init()) {
    (void)puts("--<FAILED]");
    return -1;
  }
  (void)puts("--<DONE]");

  /* Initialize imaging addon */
  (void)fputs("[al_init_image_addon>--", stdout);
  if(!al_init_image_addon()) {
    (void)puts("--<FAILED]");
    return -2;
  }
  (void)puts("--<DONE]");

  /* Initialize truetype fonts addon */
  (void)fputs("[al_init_ttf_addon>--", stdout);
  if(!al_init_ttf_addon()){
    (void)puts("--<FAILED]");
    return -3;
  }
  (void)puts("--<DONE]");

  /* Prepare event queue */
  (void)fputs("[al_create_event_queue>--", stdout);
  renderer.main_queue = al_create_event_queue();

  if(!renderer.main_queue){
    (void)puts("--<FAILED]");
    return -4;
  }
  (void)puts("--<DONE]");

  /* Get an input/event source from keyboard */
  (void)fputs("[al_install_keyboard>--", stdout);
  if(!al_install_keyboard()){
    (void)puts("--<FAILED]");
    al_destroy_event_queue(renderer.main_queue);
    return -5;
  }
  (void)puts("--<DONE]");

  (void)fputs("[al_get_keyboard_event_source>--", stdout);
  renderer.keyboard_source = al_get_keyboard_event_source();
  if(!renderer.keyboard_source){
    (void)puts("--<FAILED]");
    al_destroy_event_queue(renderer.main_queue);
    return -6;
  }
  (void)puts("--<DONE]");

  al_register_event_source(renderer.main_queue, renderer.keyboard_source);

  /* Load the font */
  (void)fputs("[al_load_ttf_font>--", stdout);
  renderer.main_font = al_load_ttf_font("/usr/share/fonts/corefonts/verdana.ttf", 16, 0);
  if(!renderer.main_font){
    (void)puts("--<FAILED]");
    al_unregister_event_source(renderer.main_queue, renderer.keyboard_source );
    al_destroy_user_event_source(renderer.keyboard_source);
    al_destroy_event_queue(renderer.main_queue);
    return -7;
  }
  (void)puts("--<DONE]");

  renderer.view_width = (view_width == 0) ? 800 : view_width;
  renderer.view_height = (view_height == 0) ? 600 : view_height;

  /* Enable multisampling */
  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

  /* Linear interpolation and mipmapping */
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MIPMAP);

  (void)fputs("[al_create_display>--", stdout);
  renderer.display = al_create_display(renderer.view_width, renderer.view_height);
  if(!renderer.display) {
    (void)puts("--<FAILED]");
    al_unregister_event_source(renderer.main_queue, renderer.keyboard_source);
    al_destroy_user_event_source(renderer.keyboard_source);
    al_destroy_event_queue(renderer.main_queue);
    return -8;
  }
  (void)puts("--<DONE]");

  /* Get input source from display */
  (void)fputs("[al_get_display_event_source>--", stdout);
  renderer.display_source = al_get_display_event_source(renderer.display);
  if(!renderer.display_source){
    al_show_native_message_box(renderer.display, "Error", "Error", "Failed to setup display event source!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
    al_unregister_event_source(renderer.main_queue, renderer.keyboard_source);
    al_destroy_user_event_source(renderer.keyboard_source);
    al_destroy_event_queue(renderer.main_queue);
    return -9;
  }
  (void)puts("--<DONE]");

  al_register_event_source(renderer.main_queue, renderer.display_source);

  /* Prepare layers */
  for(; i < RENDER_NUM_LAYERS; i++){
    renderer.layers[i] = al_create_bitmap(renderer.view_width, renderer.view_height);
    al_identity_transform(&(renderer.layer_transforms[i]));
  }

  return 0;
}

/* Add transformation to the layer */
void add_trans_to_layer(ALLEGRO_TRANSFORM *transform, unsigned char target_layer){
  if(transform){
    al_compose_transform(&(renderer.layer_transforms[target_layer]), transform);
  }
}

void apply_trans_to_layer(ALLEGRO_TRANSFORM *transform, unsigned char target_layer){
  if(transform){
    al_copy_transform(&(renderer.layer_transforms[target_layer]), transform);
  }
}

/*
inline cpBool have_to_render(ENT_NODE *tested){
  if(tested->position.x < renderer.view_width+ ){

  }
}
*/

/* TODO: Fix colour assignment for text
 * Render object to specific layer/bitmap
 */
inline void render_layers(void){
  ENT_PHYS_DYNAMIC *temp_phys_dynamic = NULL;
  ENT_PHYS_STATIC *temp_phys_static = NULL;
  ENT_NOPHYS_STATIC *temp_nophys_static = NULL;
  ENT_NOPHYS_DYNAMIC *temp_nophys_dynamic = NULL;
  ENT_NOPHYS_TEXT *temp_text = NULL;
  ENT_NOPHYS_PROGBAR *temp_prog = NULL;

  DL_FOREACH(dynamic_phys_body_list, temp_phys_dynamic){
    if(temp_phys_dynamic->bitmap){
      al_set_target_bitmap(renderer.layers[temp_phys_dynamic->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_phys_dynamic->layer]));
      al_draw_scaled_rotated_bitmap(temp_phys_dynamic->bitmap, al_get_bitmap_width(temp_phys_dynamic->bitmap)/2, al_get_bitmap_height(temp_phys_dynamic->bitmap)/2, cpBodyGetPosition(temp_phys_dynamic->body).x, cpBodyGetPosition(temp_phys_dynamic->body).y, temp_phys_dynamic->body_width/al_get_bitmap_width(temp_phys_dynamic->bitmap), temp_phys_dynamic->body_height/al_get_bitmap_height(temp_phys_dynamic->bitmap), (((int)cpBodyGetAngle(temp_phys_dynamic->body) % 360) / 360.0) * 255, 0 );
    }
  }

  DL_FOREACH(static_phys_body_list, temp_phys_static){
    if(temp_phys_static->bitmap){
      al_set_target_bitmap(renderer.layers[temp_phys_static->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_phys_static->layer]));
      al_draw_scaled_rotated_bitmap(temp_phys_static->bitmap, al_get_bitmap_width(temp_phys_static->bitmap)/2, al_get_bitmap_height(temp_phys_static->bitmap)/2, cpBodyGetPosition(temp_phys_static->body).x, cpBodyGetPosition(temp_phys_static->body).y, temp_phys_static->body_width/al_get_bitmap_width(temp_phys_static->bitmap), temp_phys_static->body_height/al_get_bitmap_height(temp_phys_static->bitmap), (((int)cpBodyGetAngle(temp_phys_static->body) % 360) / 360.0) * 255, 0 );
    }
  }

  DL_FOREACH(static_nophys_body_list, temp_nophys_static){
    if(temp_nophys_static->bitmap){
      al_set_target_bitmap(renderer.layers[temp_nophys_static->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_nophys_static->layer]));
      al_draw_scaled_bitmap(temp_nophys_static->bitmap, 0, 0, al_get_bitmap_width(temp_nophys_static->bitmap), al_get_bitmap_height(temp_nophys_static->bitmap), temp_nophys_static->position_x, temp_nophys_static->position_y, renderer.view_width, renderer.view_height, 0 );
    }
  }

  DL_FOREACH(dynamic_nophys_body_list, temp_nophys_dynamic){
    if(temp_nophys_static->bitmap){
      al_set_target_bitmap(renderer.layers[temp_nophys_dynamic->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_nophys_dynamic->layer]));
      al_draw_scaled_bitmap(temp_nophys_dynamic->bitmap, 0, 0, al_get_bitmap_width(temp_nophys_dynamic->bitmap), al_get_bitmap_height(temp_nophys_dynamic->bitmap), temp_nophys_dynamic->position_x, temp_nophys_dynamic->position_y, renderer.view_width, renderer.view_height, 0 );
    }
  }

  DL_FOREACH(nophys_text_list, temp_text){
    if(temp_text->string){
      al_set_target_bitmap(renderer.layers[temp_text->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_text->layer]));
      al_draw_text(renderer.main_font, al_map_rgb(241, 142, 0), temp_text->position_x, temp_text->position_y, 0, temp_text->string);
    }
  }

  DL_FOREACH(nophys_progress_list, temp_prog){
    if(temp_prog->monitored_value){
      al_set_target_bitmap(renderer.layers[temp_prog->layer]);
      al_use_transform(&(renderer.layer_transforms[temp_prog->layer]));
      al_draw_rectangle(temp_prog->position_x - 1, temp_prog->position_y - 2, temp_prog->position_x + temp_prog->length + 2, temp_prog->position_y + temp_prog->height + 1, al_map_rgb(241, 142, 0),1);
      al_draw_filled_rectangle(temp_prog->position_x, temp_prog->position_y, temp_prog->position_x + *(temp_prog->monitored_value) * temp_prog->length, temp_prog->position_y + temp_prog->height, al_map_rgb(241, 142, 0));
    }
  }
}

/* Preload a bitmap ad then return the ID of it */
int precache_bitmap(char *filename){
  BITMAP_PLAIN *new_bitmap = NULL;

  if(filename == NULL){
    return -1;
  }

  new_bitmap = calloc(1, sizeof(BITMAP_PLAIN));

  if(new_bitmap == NULL){
    return -2;
  }

  new_bitmap->bitmap = al_load_bitmap_flags(filename, ALLEGRO_VIDEO_BITMAP);

  if(new_bitmap->bitmap == NULL){
    return -2;
  }

  DL_APPEND(plaint_bitmap_list, new_bitmap);
  bitmap_counter++;

  return (bitmap_counter - 1);
}

/* Draw all prepared bitmaps to the main display */
void render_finalize_and_draw(){
  unsigned char i = 0;
  al_set_target_bitmap(al_get_backbuffer(renderer.display));

  al_clear_to_color(al_map_rgb(0,0,0));
  
  for(i = 0; i < RENDER_NUM_LAYERS; i++){
    al_draw_bitmap(renderer.layers[i], 0.0f, 0.0f, 0);
  }

  al_flip_display();
  
  for(i = 0; i < RENDER_NUM_LAYERS; i++){
    al_set_target_bitmap(renderer.layers[i]);
    al_clear_to_color(al_map_rgba(0,0,0,0));
  }
  
  al_set_target_bitmap(al_get_backbuffer(renderer.display));
}

/* Stop all rendering subsystems */
void stop_render(void){
  unsigned char i = 0;

  (void)puts("Shutting down renderer");
  for(;i < RENDER_NUM_LAYERS; i++){
    al_destroy_bitmap(renderer.layers[i]);
  }

  al_unregister_event_source(renderer.main_queue, renderer.display_source);
  al_unregister_event_source(renderer.main_queue, renderer.keyboard_source);
  al_destroy_user_event_source(renderer.display_source);
  al_destroy_user_event_source(renderer.keyboard_source);
  
  al_destroy_event_queue(renderer.main_queue);
  
  al_destroy_font(renderer.main_font);
  
  al_destroy_display(renderer.display);
  (void)puts("Renderer shutdown successful");
}
