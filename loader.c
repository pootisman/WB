#include <stdio.h>
#include "loader.h"
#include "interfacer.h"

int load_level(char *filename){
  FILE *level = NULL;

  if(!filename){
    return -1;
  }

  level = fopen(filename, "r");

  if(!level){
    return -2;
  }


}
