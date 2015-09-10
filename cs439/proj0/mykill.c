#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage: %s <pid>\n", argv[0]);
        exit(0);
    }

    pid_t pid = atoi(argv[1]);

    int k = kill(pid, SIGUSR1);

    if (k == -1) {
        printf("%s failed: no such process", argv[0]);
    }

    return 0;
}
