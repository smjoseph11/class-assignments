#include <stdio.h>
#include "shade.h"
int
main(int argc, char **argv){
  int width = atoi(argv[1]);
  int height = atoi(argv[2]);
  printf("%d %d\n", width, height);
  char c;
  char out; 
  int i;
  int j;
  for(i = 0; i<height; i++){
    for(j = 0; j<width; j++){
        shade(getchar(), &out);
        printf("%c", out);
      }
      printf("\n");
    }  
  return 0;
}
