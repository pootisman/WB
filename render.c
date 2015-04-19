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
inline void render_node(ENT_NODE *object){
  if(object == NULL){
    (void)puts("Can't render NULL!");
    return;
  }

  if(object->bitmap == NULL && object->string == NULL){
    return;
  }

  /* Do we have to render the object at all?*/
  if(1){
    al_set_target_bitmap(renderer.layers[object->layer]);
    al_use_transform(&(renderer.layer_transforms[object->layer]));

    if(object->bitmap != NULL && object->body != NULL){
      al_draw_scaled_rotated_bitmap(object->bitmap, al_get_bitmap_width(object->bitmap)/2, al_get_bitmap_height(object->bitmap)/2, cpBodyGetPosition(object->body).x, cpBodyGetPosition(object->body).y, object->body_width/al_get_bitmap_width(object->bitmap), object->body_height/al_get_bitmap_height(object->bitmap), (((int)cpBodyGetAngle(object->body) % 360) / 360.0) * 255, 0 );
    }else if(object->bitmap != NULL){
      al_draw_scaled_bitmap(object->bitmap, 0, 0, object->bitmap_width, object->bitmap_height, object->position.x, object->position.y, renderer.view_width, renderer.view_height, 0);
    }else if(object->string != NULL){
      al_draw_text(renderer.main_font, al_map_rgb(241, 142, 0), object->body_width, object->body_height, ALLEGRO_ALIGN_LEFT, object->string);
    }else if(object->monitored_value_pointer != NULL){
      al_draw_filled_rectangle(object->position.x, object->position.y, object->bitmap_width * *(object->monitored_value_pointer), object->bitmap_height, al_map_rgb(255 * *(object->monitored_value_pointer), 255 * *(object->monitored_value_pointer), 255 * *(object->monitored_value_pointer)));
    }
  }
}

void render_layers(void){
  
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
