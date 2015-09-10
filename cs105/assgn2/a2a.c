#include <stdio.h>

int 
main(int argc, char **argv)
{ 
  int x = 0x0; 
  int c = getchar();
  int i;
  switch(c) {
  case '0':
    x |= 0x3F; 
    break;
  case '1':
    x |= 0x06; 
    break;
  case '2':
    x |= 0x5B;
    break;
  case '3':
    x |= 0x4F;
    break;
  case '4':
    x |= 0x66;
    break;
  case '5':
    x |= 0x6D;
    break;
  case '6':
    x |= 0x7D;
    break;
  case '7':
    x |= 0x07;
    break;
  case '8': 
    x |= 0x7F;
    break;
  case '9':
    x |= 0x6F;
    break;
  default:
    printf("Not a valid number\n");
    break;
 }
   i = 7;
   while(i > 0){
     if(x & 1)
       printf("1");
     else 
       printf("0");
     x >>=1;
     i--;
  }
  printf("\n");
}

