#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

#define EIGHT_MEGABYTES 8 * 1024 * 1024

tid_t process_execute (const char *);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
void parseline (const char *, char **);
bool load_page (void *);
bool install_page (void *, void *, bool );
#endif /* userprog/process.h */
