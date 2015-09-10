#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "lib/stdbool.h"
#include "threads/thread.h"

void syscall_init (void);
bool valid_ptr(char *);
void sys_exit(int*, struct thread *, bool);
#endif /* userprog/syscall.h */
