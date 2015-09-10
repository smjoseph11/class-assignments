#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "lib/kernel/console.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
#include "threads/synch.h"
static void syscall_handler (struct intr_frame *);

  static bool
valid_fd (int fd, struct thread *t)
{
  return (fd >= 2 && fd < FILES_MAX && t->file_owner[fd] != NULL); 
}

  bool
valid_ptr (char *buff)
{
  return (is_user_vaddr(buff) && ((pagedir_get_page(active_pd(), buff) != NULL) || (page_lookup(thread_current(), ((uint32_t)buff & ~PGMASK)))));
}

//   bool
// valid_ptr (char *buff)
// {
//   uint32_t page = (uint32_t)buff & ~PGMASK;
//   return (is_user_vaddr (buff) && page_lookup (page) != NULL);
// //      return true;
// }

/* Sets exit status and deallocates all resources. Turns into a zombie until the parent dies
 * or reaps it. Then exits. */
  void
sys_exit (int* lesp, struct thread *t, bool is_bad)
{
    if (DEBUG) {
        printf ("exit\n");
    }
  /* TODO Free page table memory */
  struct thread *parent = t->parent;

  if (is_thread (parent)){
    lesp++; 
    if (is_bad){
      t->exit_status = -1;
    }
    else {
      t->exit_status = *lesp;
      if (t->exit_status < -1){
        t->exit_status = -1;
      }
    }
    t->is_dead = true;
  /* Free more memory */
  lock_acquire (get_frame_lock ());
  free_page_table ();
  lock_release (get_frame_lock ());
    /* Synchronization for process waiting */
    sema_up(&parent->wait_on_child_sema);
    sema_down(&t->child_can_die_sema);


  }
  /* Prints termination message */
  printf("%s: exit(%d)\n", t->name, t->exit_status);

  /* Close all open file */
  int i;
  for (i = 0; i < FILES_MAX; i++){
    if (t->file_owner[i] != NULL){
      file_close(t->file_owner[i]);
      t->file_owner[i] = NULL;
      t->files_open--;
    }
  }

  if (t->file != NULL){
    file_close(t->file);
    t->file_owner[i] = NULL;
  }
  struct list_elem *e;
  struct list *all_list = thread_all_list ();

  struct thread *index = NULL;

  /* Let your children die */
  for (e = list_begin (all_list); e != list_end (all_list); e = list_next (e)) {
    index = list_entry (e, struct thread, allelem);
    if (index->parent == t) {
      sema_up(&index->child_can_die_sema);
    }
  }
//   printf("Welcome to syscall exit\n");
  thread_exit();
  printf("Exit failed.\n");
}

  void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

  static void
syscall_handler (struct intr_frame *f UNUSED) 
{
//     printf("Welcome back to syscall\n");
  int fd;
  struct thread *t = thread_current ();
  char* filename;

  int* lesp = (int *) f->esp;
//     printf("Signal was %d\n", *lesp);
  if (!valid_ptr ((char *)lesp)){
    sys_exit (lesp, t, true);
  }

  if (*lesp == SYS_HALT) {
    shutdown_power_off();

  } else if (*lesp == SYS_EXIT) {
    sys_exit (lesp, t, false);

  } else if (*lesp == SYS_EXEC) {
    lesp++;

    char* cmdline = (char *) *lesp;

    const int MAXLINE = 128;
    char *argv[MAXLINE]; /* Our arguments */

    if (!valid_ptr ((char *) *lesp)) {
      f->eax = -1;
      return;
    }

    parseline (cmdline, argv);

    if (argv[0] == NULL) {
      f->eax = -1;
      return;
    }

    struct file *file = filesys_open (argv[0]);

    if (file == NULL) {
      f->eax = -1;
      return;
    }
    file_close (file);
    f->eax = process_execute (cmdline);
    sema_down(&t->wait_on_child_load_sema);

    if (!t->child_valid){
      f->eax = -1;
      return;
    }
  } else if (*lesp == SYS_WAIT) {
    lesp++;
    int pid = *lesp;

    f->eax = process_wait (pid);


  } else if (*lesp == SYS_CREATE) {
    lesp++; /* Get filename pointer */
    /* Make sure we're valid */
    if (!valid_ptr ((char *) *lesp)){
      f->eax = false;
      sys_exit (lesp, t, true);
    }
    filename = (char *) *lesp;

    lesp ++; /* File size */
    if (filename != NULL){
      f->eax = filesys_create (filename, (int32_t) *lesp);
    }

    /* SYS_REMOVE */
  } else if (*lesp == SYS_REMOVE) {
    lesp++; /* Get filename pointer */
    /* Make sure we're valid */
    if (!valid_ptr ((char *) *lesp)){
      f->eax = false;
      sys_exit (lesp, t, true);
    }
    filename = (char *) *lesp;

    lesp++; /* File size */
    if (filename != NULL) {
      f->eax = filesys_remove (filename);
    } else {
      f->eax = false;
      sys_exit (lesp, t, true);
    }

  } else if (*lesp == SYS_OPEN) {
    lesp++; /* Get filename pointer */
    /* Make sure we're valid */
    if (!valid_ptr ((char *) *lesp)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

    filename = (char *) *lesp;

    if (filename == NULL || strcmp(filename, "") == 0){
      f->eax = -1;
      return;
    }

    if (t->files_open >= FILES_MAX){
      f->eax = -1;
      return;
    }
    struct file *file = filesys_open(filename);
    t->files_open++;
    if (file == NULL) {
      f->eax = -1;
      return;
    }

    int i;
    for(i = 2; i < FILES_MAX; i++){
      if (t->file_owner[i] == NULL){
        t->file_owner[i] = file;
        f->eax = i;
        break;
      }
    }

  } else if (*lesp == SYS_FILESIZE) {
    lesp++; 
    fd = (int) *lesp;

    if (valid_fd(fd, t)) {
      f->eax = file_length ((struct file *) t->file_owner[fd]);
    }
    else {
      f->eax = -1;
      sys_exit (lesp, t, true);
    }


  } else if (*lesp == SYS_READ) {
//     printf ("Welcome to sys_read.\n");
    lesp++; 
    fd = (int) *lesp;
    lesp++;
    char *buff = (char *) *lesp;

    if (!valid_ptr (buff)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

    lesp++;
    off_t size = (off_t) *lesp;

    /* Check time */

    if (valid_fd (fd, t)) {
      f->eax = file_read ((struct file *) t->file_owner[fd], (void *) buff, size);
    } else {
      f->eax = -1;
      sys_exit (lesp, t, true);
    }


  } else if (*lesp == SYS_WRITE) {
    lesp++;
    int fd = *lesp;
    lesp++;
    char *buff = (char *) *lesp;
    lesp++;
    unsigned size = *lesp; 

    if (!valid_ptr (buff)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

    if(fd == 1) {
      putbuf(buff, size);
    } else if (valid_fd (fd, t)) {
      f->eax = file_write ((struct file *) t->file_owner[fd], (void *) buff, size);
    } else {
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

  } else if (*lesp == SYS_SEEK) {
    lesp++;
    int fd = *lesp;
    lesp++;
    off_t pos = (off_t) *lesp;

    if (valid_fd (fd, t) && pos >= 0) {
      file_seek ((struct file *) t->file_owner[fd], pos);
    }

  } else if (*lesp == SYS_TELL) {
    lesp++;
    int fd = *lesp;

    if (valid_fd (fd, t)) {
      f->eax = (unsigned) file_tell ((struct file *) t->file_owner[fd]);
    } else {
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

  } else if (*lesp == SYS_CLOSE) {
    lesp++;
    fd = (int) *lesp; /* Get fd */

    if (valid_fd (fd, t)) {
      file_close (t->file_owner[fd]);
      t->files_open--;
      t->file_owner[fd] = NULL;
    }
  }
} 
