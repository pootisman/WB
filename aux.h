#ifndef _AUX_
#define _AUX_

#include <math.h>
#include <chipmunk/chipmunk.h>

/* Auxiliary math */
/* Distance between two points */
static inline double dist(cpVect point_A, cpVect point_B){
  return sqrt((point_A.x - point_B.x)*(point_A.x - point_B.x) + (point_A.y - point_B.y)*(point_A.y - point_B.y));
}

/* A vector of length 1, pointing from A to B */
static inline cpVect pointer_vect(cpVect point_A, cpVect point_B){
  char x = 1, y = 1;

  if(point_A.x < point_B.x){
    x = -1;
  }else if(point_A.x > point_B.x){
    x = 1;
  }else{
    x = 0;
  }

  if(point_A.y < point_B.y){
    y = -1;
  }else if(point_A.y > point_B.y){
    y = 1;
  }else{
    y = 0;
  }

  return cpv(x * sqrt(((point_A.x - point_B.x)*(point_A.x - point_B.x))/(dist(point_A, point_B)*dist(point_A, point_B))), y * sqrt(((point_A.y - point_B.y)*(point_A.y - point_B.y))/(dist(point_A, point_B)*dist(point_A, point_B))));
}

/* Vector of length equal to multiplier */
static inline cpVect pointer_vect_mul(cpVect point_A, cpVect point_B, double multiplier){
  cpVect result = cpv(pointer_vect(point_A, point_B).x * multiplier, pointer_vect(point_A, point_B).y * multiplier);
  return result;
}

#endif
