#include "filesys/filesys.h"
#include <debug.h>
#include <stdio.h>
#include <string.h>
#include "filesys/file.h"
#include "filesys/free-map.h"
#include "filesys/inode.h"
#include "filesys/directory.h"
#include "threads/thread.h"
#include "threads/palloc.h"
/* Partition that contains the file system. */
struct block *fs_device;

static void do_format (void);

/* Initializes the file system module.
   If FORMAT is true, reformats the file system. */
void
filesys_init (bool format) 
{
  fs_device = block_get_role (BLOCK_FILESYS);
  if (fs_device == NULL)
    PANIC ("No file system device found, can't initialize file system.");

  inode_init ();
  free_map_init ();

  if (format) 
    do_format ();

  free_map_open ();
}

/* Shuts down the file system module, writing any unwritten data
   to disk. */
void
filesys_done (void) 
{
  free_map_close ();
}

/* Creates a file named NAME with the given INITIAL_SIZE.
   Returns true if successful, false otherwise.
   Fails if a file named NAME already exists,
   or if internal memory allocation fails. */
bool
filesys_create (struct dir *dir, const char *name, off_t initial_size, bool is_dir) 
{
  block_sector_t inode_sector = 0;
  bool success;
  if (is_dir) { 
    success = (dir != NULL
                    && !dir->inode->removed
                    && free_map_allocate (1, &inode_sector)
                    && dir_create (inode_sector, initial_size)
                    && dir_add (dir, name, inode_sector));

    if (!success) {
      return success;
    }

    struct dir *new_dir = dir_open_from_sector (inode_sector);

    dir_add (new_dir, ".", inode_sector);
    dir_add (new_dir, "..", dir->inode->sector);

    dir_close (new_dir);

  } else {
    success = (dir != NULL
                    && !dir->inode->removed
                    && free_map_allocate (1, &inode_sector)
                    && inode_create (inode_sector, initial_size)
                    && dir_add (dir, name, inode_sector));
  }
  if (!success && inode_sector != 0) 
    free_map_release (inode_sector, 1);
  dir_close (dir);

  return success;
}

/* Opens the file with the given NAME.
   Returns the new file if successful or a null pointer
   otherwise.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
struct file *
filesys_open (const char *name)
{
  struct dir *dir = dir_open_root();
  struct inode *inode = NULL;
   if (dir != NULL)
    dir_lookup (dir, name, &inode);
  dir_close (dir);

  if (inode == NULL) {
      DEBUG("In filesys_open: dir_lookup returned NULL inode.");
      return NULL;
  }
  
  return file_open (inode);
}


struct file *
filesys_open_from_dir (const char *name, struct dir *dir)
{
  if (dir->inode->removed){
    return NULL;
  }
  struct inode *inode = NULL;
   if (dir != NULL)
    dir_lookup (dir, name, &inode);

  if (inode == NULL) {
      DEBUG("In filesys_open: dir_lookup returned NULL inode.");
      return NULL;
  }
  
  return file_open (inode);
}

/* Deletes the file named NAME.
   Returns true if successful, false on failure.
   Fails if no file named NAME exists,
   or if an internal memory allocation fails. */
bool
filesys_remove (const char *name) { 
// struct thread *t = thread_current();
// struct dir *dir = dir_open_from_sector(t->working_directory);
   struct dir *dir = dir_open_root();
  bool success = dir != NULL && dir_remove (dir, name);
  dir_close (dir); 

  return success;
}

bool
filesys_remove_from_dir (const char *name, struct dir *dir) { 
  ASSERT(dir != NULL);
    bool success = dir != NULL && dir_remove (dir, name);
  dir_close (dir); 

  return success;
}
/* Formats the file system. */
static void
do_format (void)
{
  printf ("Formatting file system...");
  free_map_create ();
  if (!dir_create (ROOT_DIR_SECTOR, 16))
    PANIC ("root directory creation failed");
  free_map_close ();
  printf ("done.\n");
}

bool
filesys_exists (const char *name)
{
  struct dir *dir = dir_open_root ();
  struct inode *inode = NULL;
  bool exists = false;

  if (dir != NULL) {
    exists = dir_lookup (dir, name, &inode);
  }
  dir_close (dir);

  return exists;
}
