#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

const int STDOUT = 1;

// Noah driving now
void intr_handler(int sig){
    if (sig == SIGINT) {
        ssize_t bytes = write(STDOUT, "Nice try.\n", 10);

        if (bytes != 10) {
            exit(-999);
        }
    } else if (sig == SIGUSR1) {
        ssize_t bytes = write(STDOUT, "exiting\n", 8);

        if (bytes != 8) {
            exit(-999);
        }
        
        exit(1);
    }
}


// stop Noah driving, Max driving now


/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * second.
 */

int main(int argc, char **argv)
{
    Signal(SIGINT, intr_handler);
    Signal(SIGUSR1, intr_handler);

    struct timespec tim = {1, 0};

    printf("%d\n", getpid());

    while (1) {
        printf("Still here\n");
        nanosleep(&tim, NULL);
    }

    return 0;
}

// end Max driving
