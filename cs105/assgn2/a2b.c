#include <stdio.h>

int main(int argc, char **argv)
{
  char c;
  char v = 1;
  char ans = 0;
  int i = 6;
  int j;
  while((c = getchar()) != EOF)
  {
    if(c == '1'){
      ans += v << i;
    }
  /*
    if(c == '1'){
      int j;
      int pow = 1;
      for(j = i; j>0; j--){
         pow *= 2;
      }
      printf("%d\n", pow);
      ans += pow;
    }
   */
    i--;
  }  
  printf("%c\n", ans);
}
