/* 
 * msh - A mini shell program with job control
 * 
 * <Put your name and login ID here>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "util.h"
#include "jobs.h"

#define STDOUT 1

/* Global variables */
int verbose = 0;            /* if true, print additional output */

extern char **environ;      /* defined in libc */
static char prompt[] = "msh> ";    /* command line prompt (DO NOT CHANGE) */
static struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
void usage(void);
void sigquit_handler(int sig);

/*
 * Error-handling wrappers
 */
pid_t Fork(void)
{
pid_t pid;
if ((pid = fork()) < 0)
unix_error("Fork error");
return pid;
}



/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    initjobs(jobs);
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
    // Noah driving now
	char *argv[MAXLINE]; /*Argument for execve*/
	int bg = parseline(cmdline, argv); /*True if the program should run in the background*/
    	int bi = builtin_cmd(argv); /*True if the command line is a built-in command*/
    	sigset_t mask; /*Mask for signal blocking*/
    if (!bi) {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL); /* Block SIGCHLD to avoid race cases*/

         pid_t pid = Fork();
        
        if (pid == 0) {
            setpgid(0, 0); /*Give children their own group id*/
            sigprocmask(SIG_UNBLOCK, &mask, NULL); /*Unblock SIGCHLD to receive signals*/
            
            int r = execve(argv[0], argv, environ); /*Non-negative if a command is input*/
        
            if (r < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        } else {

            // end Noah driving, Max driving now
            if (!bg) {
                addjob(jobs, pid, FG, cmdline);
                sigprocmask(SIG_UNBLOCK, &mask, NULL); /*Unblock SIGCHLD to receive signals*/
                waitfg(pid); /*Wait for a foreground process*/
            } else {
                addjob(jobs, pid, BG, cmdline);
                sigprocmask(SIG_UNBLOCK, &mask, NULL); /*Unblock SIGCHLD to receive signals*/
                struct job_t* job = getjobpid(jobs, pid);
                printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
            }
        }
    }

    return;
}


/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately. 
 * Return 1 if a builtin command was executed; return 0
 * if the argument passed in is *not* a builtin command.
 */
int builtin_cmd(char **argv) 
{
    if (argv[0] == NULL) {
        return 1; /*Ignore empty lines*/

    } else if (strcmp(argv[0], "quit") == 0) {
        exit(0);

    } else if(strcmp(argv[0],"jobs") == 0){
        listjobs(jobs);
        return 1;

    } else if (strcmp(argv[0], "bg") == 0 || strcmp(argv[0], "fg") == 0) {
        do_bgfg(argv);
        return 1;
    }

    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    char* temp;
    struct job_t* job;

	
    if (argv[1][0] == '%') { /*Job Id*/
        temp = argv[1];
        memmove(temp, temp+1, strlen(temp)); /*Remove leading % sign*/
        int jid = atoi(temp);

        if (jid == 0) {
            printf("%s: argument must be a PID or %%jobid\n", argv[0]);
            return ;
        }

        job = getjobjid(jobs, jid);

        if (job == NULL) {
            printf("%s: No such job\n", argv[1]);
            return;
        }
    } else { /*Pid*/
        pid_t pid = atoi(argv[1]);

        if (pid == 0) {
            printf("%s: argument must be a PID or %%jobid\n", argv[0]);
            return ;
        }

        job = getjobpid(jobs, pid);

        if (job == NULL) {
            printf("(%s): No such process\n", argv[1]);
            return;
        }
    }
    
    if (strcmp(argv[0], "bg") == 0) {
        job->state = BG;
        kill(-job->pid, SIGCONT); /*Signal stopped processes to continue*/
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    } else if (strcmp(argv[0], "fg") == 0) {
        job->state = FG;
        kill(-job->pid, SIGCONT);/*Signal stopped processes to continue*/
        waitfg(job->pid);
    }
}

// end Max driving, Noah driving now

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */

void waitfg(pid_t pid)
{
    while (fgpid(jobs)) {
        sleep(1);
    }
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */

// Noah driving now
void sigchld_handler(int sig) 
{
    pid_t pid;
    int status;
    int newsig;
    int i;
    const int MAXMSG = 43;
    char msg[MAXMSG];

    for (i = 0; i < MAXMSG; i++) {
        msg[i] = 0;
    }

/*Waiting is handled in waitfg(). Does not wait for stopped or background processes.*/
    if ((pid = waitpid(-1, &status, WUNTRACED|WNOHANG)) < 0){
		unix_error("waitpid error");
	} 
    if (pid > 0) {
        struct job_t* job = getjobpid(jobs, pid);
        if (WIFSIGNALED(status)){
            newsig = WTERMSIG(status);

            sprintf(msg, "Job [%d] (%d) terminated by signal %d\n", job->jid, job->pid, newsig);
            write(STDOUT, msg, MAXMSG);
            fflush(stdout);

            deletejob(jobs, job->pid);
        } else if (WIFSTOPPED(status)){
            newsig = WSTOPSIG(status);
            job->state = ST;

            sprintf(msg, "Job [%d] (%d) stopped by signal %d\n", job->jid, job->pid, newsig);
            write(STDOUT, msg, MAXMSG);
            fflush(stdout);
        } else {
            deletejob(jobs, job->pid);
        }
    }
}

// end Noah driving

// Max driving now

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
    pid_t pid;
    if ((pid = fgpid(jobs)) != 0){
        kill(-pid, SIGINT);
    }
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    pid_t pid;
    if ((pid = fgpid(jobs)) != 0){
        kill(-pid, SIGTSTP);
    }
}


// end Max driving
/*********************
 * End signal handlers
 *********************/



/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}



