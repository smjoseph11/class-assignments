#include <stdio.h>
#include "shade.h"

int
shade(char c, char *out){
  switch(c){
    case '0': *out = ' '; break;
    case '1': *out = '.'; break;
    case '2': *out = ':'; break;
    case '3': *out = 'c'; break;
    case '4': *out = 'o'; break;
    case '5': *out = 'C'; break;
    case '6': *out = 'O'; break;
    case '7': *out = '8'; break;
    case '8': *out = '@'; break;
  }
  return 1;
}
