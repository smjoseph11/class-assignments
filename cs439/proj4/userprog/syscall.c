#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "lib/kernel/console.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "filesys/inode.h"
#include "threads/vaddr.h"
#include "userprog/pagedir.h"
#include "threads/palloc.h"
struct lock sys_lock;
enum command {MKDIR, CHDIR, CREATE, OPEN, REMOVE};

static void syscall_handler (struct intr_frame *);
void get_dir (char *,enum command, struct intr_frame *, int32_t);
bool is_dir(struct file *);
struct file * get_file(int, struct intr_frame *, void *);

  static bool
valid_fd (int fd, struct thread *t)
{
  return (fd >= 2 && fd < FILES_MAX && t->file_owner[fd] != NULL); 
}

  bool
valid_ptr (char *buff)
{
  return (is_user_vaddr(buff) && pagedir_get_page(active_pd(), buff) != NULL);
}


  void
get_dir (char *path,  enum command cmd, struct intr_frame *f, int32_t size)
{
  struct thread *t = thread_current ();
  struct dir *current_dir;
  if (path[0] == '/') {
    /* Absolute Path */
    current_dir = dir_open_root ();
    path++;
  } else {
    /* Relative path */
    current_dir = dir_open_from_sector(t->working_directory);
  }

  char *fn_copy;
  fn_copy = palloc_get_page (0);
  if (fn_copy == NULL) {
    PANIC("PALLOC WENT CRAZY HELP"); 
  }

  strlcpy (fn_copy, path, PGSIZE);
  /* Replace all '/' with ' ' so we can feed it to parseline */
  char *ch = fn_copy;
  int subdirs = 1;
  for (; *ch != '\0'; ch++) {
    if (*ch == '/') {
      *ch = ' ';
      subdirs++;
    }
  }

  char *dir_tree[subdirs]; /* Our arguments */
  char *filename;
  parseline(fn_copy, dir_tree);
  palloc_free_page (fn_copy);

  struct inode *dir_inode;

  if (cmd == CHDIR) {
    DEBUG("CHDIR found");
    subdirs++;
  }

  /* Traverse path to directory */
  int i;
  for (i = 0; i < subdirs-1; i++) {
    bool exists = dir_lookup (current_dir, dir_tree[i], &dir_inode);

    if (!exists) {
      dir_close (current_dir);
      DEBUG("A subdir doesn't exist.");
      if (cmd == OPEN){
        f->eax = -1;
      }
      else{
        f->eax = false;
      }
      return;
    }

    dir_close (current_dir);
    current_dir = dir_open (dir_inode);
  }

  if (cmd == CHDIR){
    i--;
  }
  filename = dir_tree[i];

  if (filename == NULL){
    f->eax = false;
    DEBUG("filename is NULL");
    return;
  }

  if (current_dir == NULL) {
    DEBUG("The current directory is NULL.");
    f->eax = false;
    return;
  }
  if (cmd == MKDIR || cmd == CREATE) {
    if (cmd == MKDIR){
      DEBUG("MKDIR found");
      f->eax = filesys_create (current_dir, filename, 0, true);
    }
    else if (cmd == CREATE){
      DEBUG("CREATE found");
      f->eax = filesys_create (current_dir, filename, size, false);
    }
  }
  else if (cmd == CHDIR){
    DEBUG("CHDIR found");
    t->working_directory = dir_inode->sector;
    f->eax = true;
    ASSERT(dir_inode->sector == dir_inode->data.inode_disk_number);
    dir_close(current_dir);
    return;
  }else if (cmd == OPEN){
    DEBUG("OPEN found");
    if (t->files_open >= FILES_MAX){
      f->eax = -1;
      return;
    }
    struct file *file = filesys_open_from_dir(filename, current_dir);
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
  } else if (cmd == REMOVE){
    DEBUG("REMOVE found");
    char unused[NAME_MAX + 1];
    struct file *file = filesys_open_from_dir(filename, current_dir);

    if (is_dir(file)){
      DEBUG("Directory found");
      struct dir *dir = dir_open(file->inode);
      if (!dir_readdir(dir, unused)){
        f->eax = filesys_remove_from_dir (filename, current_dir);
        if (f->eax == false){
          DEBUG("Failed to remove directory");
        }
      }
      else{
        f->eax = false;
        DEBUG("Still has entries");
      }
    }
    else {
      f->eax = filesys_remove_from_dir (filename, current_dir);
      if (f->eax == false){
        DEBUG("Failed to remove file");
      }
    }
  }
}

struct file * get_file(int fd, struct intr_frame *f, void *lesp){
  struct thread *t = thread_current();
  if (!valid_fd (fd, t)) {
    f->eax = -1;
    sys_exit (lesp, t, true);
  }
  struct file *file = t->file_owner[fd];
  return file;
}

bool
is_dir(struct file *file){
  return file->inode->data.is_dir;
}
/* Sets exit status and deallocates all resources. Turns into a zombie until the parent dies
 * or reaps it. Then exits. */
  void
sys_exit (int* lesp, struct thread *t, bool is_bad)
{
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

  thread_exit();
  printf("Exit failed.\n");
}

  void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init(&sys_lock);
}

  static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int fd;
  struct thread *t = thread_current ();
  char* filename;

  int* lesp = (int *) f->esp;
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
    struct file *file = filesys_open(argv[0]);

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

    t->working_directory = t->parent->working_directory;

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
    int32_t length = (int32_t) *lesp;

    if (filename != NULL){
      get_dir(filename, CREATE, f, length);
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
    get_dir(filename, REMOVE, f, (int) *lesp);
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

  if (strcmp(filename, "/") == 0){
    DEBUG("Welcome to opening root.");
      if (t->files_open >= FILES_MAX){
        f->eax = -1;
        return;
      }
      struct dir *dir = dir_open_root();
      struct file *file = file_open(dir->inode);
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
    dir_close(dir);
  } else{
      get_dir(filename, OPEN, f, 0);
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
    lesp++; 
    fd = (int) *lesp;
    lesp++;
    char *buff = (char *) *lesp;

    if (!valid_ptr (buff)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

    lesp++;
    off_t read = (off_t) *lesp;

    if (valid_fd (fd, t)) {
      f->eax = file_read ((struct file *) t->file_owner[fd], (void *) buff, read);
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
      if(f->eax != size){
        if(VERBOSE){
          printf("size = %d\nbytes written %d\n", size, f->eax);
        }
      }
    } else {
      DEBUG("Invalid FD");
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
    int fd = (int) *lesp; /* Get fd */

    if (valid_fd (fd, t)) {
      file_close (t->file_owner[fd]);
      t->files_open--;
      t->file_owner[fd] = NULL;
    }
  } else if (*lesp == SYS_MKDIR) {
    lesp++; /* Get filename pointer */
    /* Make sure we're valid */
    if (!valid_ptr ((char *) *lesp)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }
    char* filepath = (char *) *lesp;
    if (filepath != NULL){
      get_dir (filepath, MKDIR, f, 0);
    }


  } else if (*lesp == SYS_CHDIR) {
    lesp++; /* Get filename pointer */
    /* Make sure we're valid */
    if (!valid_ptr ((char *) *lesp)){
      f->eax = -1;
      sys_exit (lesp, t, true);
    }

    char* filepath = (char *) *lesp;
    get_dir (filepath, CHDIR, f, 0);

  } else if (*lesp == SYS_READDIR){
    lesp++; 
    int fd = (int) *lesp;
    lesp++;
    struct file *file = get_file(fd, f, lesp);
    char *filename = (char *) *lesp;
    if (!is_dir(file)){
      f->eax = false;
      return;
    }
    struct dir *dir = dir_get(t->file_owner[fd]->inode);
    char name[NAME_MAX + 1];
    bool success = dir_readdir(dir, name);
    //     printf("Filename: '%s'\n", name);
    f->eax = success;
    if (!success){
      return;
    }
    dir_close(dir); 
    strlcpy(filename, name, NAME_MAX + 1);
  } else if (*lesp == SYS_ISDIR) {
    lesp++;
    int fd = (int) *lesp;
    struct file *file = get_file(fd, f, lesp);
    f->eax = is_dir(file);
  } else if (*lesp == SYS_INUMBER){
    lesp++;
    int fd = (int) *lesp;
    if (!valid_fd(fd, t)){
      if (VERBOSE){
        printf("In inumber: Your fd(%d) is not valid.\n", fd);
      }
      f->eax = -1;
      sys_exit (lesp, t, true);
    }
    return t->file_owner[fd]->inode->sector;
  }
}
