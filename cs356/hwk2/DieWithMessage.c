#include <stdio.h>
#include <stdlib.h>

void DieWithError(char *errorMessage) {
    perror(errorMessage);
    exit(1);
}


void DieWithUserMessage(const char *msg, const char *detail) {
  fputs(msg, stderr);
  fputs(": ", stderr);
  fputs(detail, stderr);
  fputc('\n', stderr);
  exit(1);
}

void DieWithSystemMessage(const char *msg) {
  perror(msg);
  exit(1);
}
