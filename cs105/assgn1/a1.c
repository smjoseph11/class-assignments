#include <stdio.h>
#include <ctype.h>

#define DIGIT 10
//This program changes all lowercase letters to uppercase letters for a given input
int
main()
{
  printf("Enter some text and I will change it to uppercase!\n");
  char c;
  int i;
  int numerals[DIGIT];
 
  for(i = 0; i<DIGIT; i++){
    numerals[i] = 0;
  }
  while ((c = getchar()) != EOF){
    c = toupper(c);
    if(c >= '0' && c<= '9'){
       numerals[c-'0']++; 
    }
  putchar(c);
  }
  for(i = 0; i<DIGIT; i++){
    printf("%d\n", numerals[i]);
  }
}
