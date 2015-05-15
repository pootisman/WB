#include "render.h"
#include "game_phys.h"
#include "game_loop.h"
#include "pre_menu.h"
#include <stdlib.h>

int main(int argc, char **argv){

  if(0 != init_render(1024, 768)){
    return EXIT_FAILURE;
  }

  if(0 != init_phys()){
    return EXIT_FAILURE;
  }

  show_greeting();
  show_description();

  request_name();

  run_loop();

  stop_game();

  return EXIT_SUCCESS;
}
