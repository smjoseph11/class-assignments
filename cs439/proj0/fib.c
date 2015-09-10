#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

const int MAX = 13;

static void doFib(int n, int doPrint);


/*
 * unix_error - unix-style error routine.
 */
inline static void 
unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * Error-handling wrapper
 */
pid_t Fork(void)
{
pid_t pid;
if ((pid = fork()) < 0)
unix_error("Fork error");
return pid;
}

int main(int argc, char **argv)
{
  int arg;
  int print;

  if(argc != 2){
    fprintf(stderr, "Usage: fib <num>\n");
    exit(-1);
  }

  if(argc >= 3){
    print = 1;
  }

  arg = atoi(argv[1]);
  if(arg < 0 || arg > MAX){
    fprintf(stderr, "number must be between 0 and %d\n", MAX);
    exit(-1);
  }

  doFib(arg, 1);

  return 0;
}

/* 
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must Fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */

// Max driving now
static void 
doFib(int n, int doPrint)
{
    pid_t pid = Fork();
    int r;

    if (pid == 0) {
        if (n == 0 || n == 1) {
            exit(n);
        } else {
            pid_t pid1 = Fork();
            pid_t pid2;

            int r1;
            int r2;

            if (pid1 != 0) {
                pid2 = Fork();
            }

            if (pid1 == 0) {
                doFib(n-1, 0);
            } else if (pid2 == 0) {
                doFib(n-2, 0);

            } else {
                waitpid(pid1, &r1, 0);
                waitpid(pid2, &r2, 0);
                exit(WEXITSTATUS(r1) + WEXITSTATUS(r2));
            }
        }
    } else {
        wait(&r);

        int status = WEXITSTATUS(r);

        if (doPrint) {
            printf("%d\n", status);
        } else {
            exit(status);
        }
    }
}
// end Max driving, start Noah

