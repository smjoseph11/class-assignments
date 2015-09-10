#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include <bitmap.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/interrupt.h"
#include <stdio.h>
#include "filesys/directory.h"
#define INODE_MAGIC 0x494e4f44

struct lock extend_lock;
struct lock length_lock;

/* filesys */
struct lv1_ind_block * inode_open_lv1(block_sector_t);
bool inode_allocate(block_sector_t, block_sector_t, struct inode_disk *);

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}


/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t
byte_to_sector (const struct inode *inode, off_t pos) 
{

    ASSERT (inode != NULL);
    ASSERT (pos >= 0);
    ASSERT (inode->data.length <= inode->data.new_length);
    off_t length = inode->data.new_length;
    block_sector_t data_sector = -1;
//     printf ("Position: %d.\n Length: %d.\n", pos, length);
    if (pos < length){
        block_sector_t sector_index = pos / BLOCK_SECTOR_SIZE;
        if (sector_index < MASTER_SIZE){
            data_sector = inode->data.direct_block[sector_index];
//              printf("sector_index = %d\n", sector_index);
        }
        else {
            sector_index -= MASTER_SIZE;
            block_sector_t ind_index = sector_index / IND_BLOCK_SIZE;
            sector_index = sector_index % IND_BLOCK_SIZE;
            struct lv1_ind_block *ind_lv1 = inode_open_lv1(inode->data.lv1_ind_block[ind_index]);
            data_sector = ind_lv1->direct_block[sector_index];
            free (ind_lv1);
        }
    if (data_sector > 50000 || data_sector < 0){
        printf ("Byte to sector called with length %d\n", bytes_to_sectors(pos));
        printf ("Returned %d\n", data_sector);
        return -1;
    }
        return data_sector;
    }
    return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void
inode_init (void) 
{
  list_init (&open_inodes);
  lock_init(&extend_lock);
  lock_init(&length_lock);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */

bool
inode_create (block_sector_t inode_disk_sector, off_t length)
{
  return inode_create_dir (inode_disk_sector, length, false);
}

bool
inode_create_dir (block_sector_t inode_disk_sector, off_t length, bool is_dir)
{
  /* Be sure to allocate a sector right before calling this function with start */
  struct inode_disk *disk_inode = NULL;
  
  bool success = false;
  ASSERT (length >= 0);

  /* If this assertion fails, the inode structure is not exactly
     one sector in size, and you should fix that. */
  ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);
  size_t sectors = bytes_to_sectors (length);
          /* DEBUG PRINT */
//           printf("Free map needs to have %d sectors. \n", sectors);
  if (bitmap_has_room(get_free_map(), sectors)){
      disk_inode = calloc (1, sizeof *disk_inode);
      
            if (disk_inode != NULL)
      {
          disk_inode->length = length;
          disk_inode->new_length = length;
          disk_inode->is_dir = is_dir;
          disk_inode->dir_pos = sizeof(struct dir_entry) * 2;
//              disk_inode->length = (IND_BLOCK_SIZE + MASTER_SIZE) * BLOCK_SECTOR_SIZE;
          disk_inode->magic = INODE_MAGIC;
          success = inode_allocate(inode_disk_sector, sectors, disk_inode);
      }
      free (disk_inode);
  }

  return success;
}

bool
inode_allocate(block_sector_t disk_inode_sector, block_sector_t sectors, struct inode_disk * disk_inode){

  struct lv1_ind_block *lv1_indirect[IND_NUM_BLOCKS];
  int i;
  for (i = 0; i < IND_NUM_BLOCKS; i++){
    ASSERT (sizeof(*lv1_indirect[i]) == BLOCK_SECTOR_SIZE);
    lv1_indirect[i] = calloc(1, sizeof(*lv1_indirect[i]));
  }
  size_t count = 0;
  block_sector_t current_sector = 2;
  while (count < sectors){
    free_map_allocate(1, &current_sector);
    static char zeros[BLOCK_SECTOR_SIZE];
    block_write (fs_device, current_sector, zeros);
    
    if (count < MASTER_SIZE){
      disk_inode->direct_block[count] = current_sector;
    }
    else if (count < MASTER_SIZE + (IND_BLOCK_SIZE * IND_NUM_BLOCKS)){
      size_t tmp = count - MASTER_SIZE;
      size_t ind_index = tmp / IND_BLOCK_SIZE;
      block_sector_t sector_index = tmp % IND_BLOCK_SIZE;
      lv1_indirect[ind_index]->direct_block[sector_index] = current_sector;
    }
    else{
        PANIC("OUT OF SPACE AHHHHHHHHHHHHHH");
    }
    count++;
  }
  /* Allocate our unwritten level 1 indirect blocks */
    for (i = 0; i < IND_NUM_BLOCKS; i++){
      free_map_allocate(1, &current_sector);
      block_write (fs_device, current_sector, lv1_indirect[i]);
      disk_inode->lv1_ind_block[i] = current_sector;
      free(lv1_indirect[i]);
    }
  /*Now that we've initialized our data_inode, we can finally write it to disc */
  disk_inode->inode_disk_number = disk_inode_sector;
  block_write (fs_device, disk_inode_sector, disk_inode);
  
  return true;

  }
bool
inode_extend_file (struct inode* inode, block_sector_t num_sectors, off_t new_length){
  struct inode_disk *disk_inode = &inode->data;
  struct lv1_ind_block *lv1_indirect[IND_NUM_BLOCKS];
  size_t count = 0;
  block_sector_t length_in_sectors = bytes_to_sectors(inode->data.length);
  num_sectors = num_sectors - length_in_sectors;
  if (num_sectors == 0){
    lock_release(&extend_lock);
    return true;
  }
  count += length_in_sectors;
  num_sectors += length_in_sectors;

  block_sector_t current_sector = 2;
  int i;
  for (i = 0; i < IND_NUM_BLOCKS; i ++){
    ASSERT(bitmap_test(get_free_map(), disk_inode->lv1_ind_block[i]));
    lv1_indirect[i] = inode_open_lv1(disk_inode->lv1_ind_block[i]);
  }
  while (count < num_sectors){
    free_map_allocate(1, &current_sector);
    static char zeros[BLOCK_SECTOR_SIZE];
    block_write (fs_device, current_sector, zeros);
    
    if (count < MASTER_SIZE){
      disk_inode->direct_block[count] = current_sector;
    }
    else if (count < MASTER_SIZE + (IND_BLOCK_SIZE * IND_NUM_BLOCKS)){
      size_t tmp = count - MASTER_SIZE;
      size_t ind_index = tmp / IND_BLOCK_SIZE;
      block_sector_t sector_index = tmp % IND_BLOCK_SIZE;
      lv1_indirect[ind_index]->direct_block[sector_index] = current_sector;
    }
    else{
        PANIC("OUT OF SPACE AHHHHHHHHHHHHHH");
    }
    count++;
  }
  /* Overwrite our level 1 indirect blocks */
    for (i = 0; i < IND_NUM_BLOCKS; i++){
      block_write (fs_device, disk_inode->lv1_ind_block[i], lv1_indirect[i]);
      free(lv1_indirect[i]);
    }
  /* Overwrite our disk_inode */
  block_write (fs_device, disk_inode->inode_disk_number, disk_inode);
  

  return true;
}
/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
  struct list_elem *e;
  struct inode *inode;

  /* Check whether this inode is already open. */
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
       e = list_next (e)) 
    {
      inode = list_entry (e, struct inode, elem);
      if (inode->sector == sector) 
        {
          inode_reopen (inode);
          return inode; 
        }
    }

  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL)
    return NULL;

  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  inode->sector = sector;
  inode->open_cnt = 1;
  inode->deny_write_cnt = 0;
  inode->removed = false;
  lock_init(&inode->write_lock);
  block_read (fs_device, inode->sector, &inode->data);
  return inode;
}
struct lv1_ind_block *
inode_open_lv1(block_sector_t sector){
    struct lv1_ind_block *lv1_indirect;
    ASSERT (sizeof *lv1_indirect == BLOCK_SECTOR_SIZE);
    lv1_indirect = malloc(sizeof *lv1_indirect);
    block_read(fs_device, sector, lv1_indirect);
    return lv1_indirect;
}


/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
  if (inode != NULL)
    inode->open_cnt++;
  return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
  return inode->sector;
}

/* Closes INODE and writes it to disk. (Does it?  Check code.)
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
  void
inode_close (struct inode *inode) 
{
  DEBUG("Welcome to inode_close");
  /* Ignore null pointer. */
  if (inode == NULL)
    return;

  struct inode_disk *disk_inode = &inode->data;

  if (intr_get_level () == INTR_ON) {
    if ((int)inode->sector < 0){
      return 0;
    }
    block_write(fs_device, inode->sector, disk_inode);
  }

  /* Release resources if this was the last opener. */
  if (--inode->open_cnt == 0)
  {
    /* Remove from inode list and release lock. */
    list_remove (&inode->elem);

    /* Deallocate blocks if removed. */
    if (inode->removed) {
      size_t count = 0;
      struct lv1_ind_block *lv1_indirect[IND_NUM_BLOCKS];
      int i;
      for (i = 0; i < IND_NUM_BLOCKS; i++) {
        lv1_indirect[i] = inode_open_lv1(disk_inode->lv1_ind_block[i]);
      }
      size_t sectors = bytes_to_sectors(inode->data.length);
      while (count < sectors) {
        if(count < MASTER_SIZE) {
          free_map_release(disk_inode->direct_block[count], 1);
        } else {
          size_t ind_index = sectors / IND_BLOCK_SIZE;
          size_t sector_index = sectors % IND_BLOCK_SIZE;
          free_map_release(lv1_indirect[ind_index]->direct_block[sector_index], 1);
        }
        count++;
      }
      for (i = 0; i < IND_NUM_BLOCKS; i++) {
        free_map_release (disk_inode->lv1_ind_block[i], 1);
      }
      free_map_release (inode->sector,1);
    } 

    free (inode);
  }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode) 
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset) 
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  uint8_t *bounce = NULL;
  off_t new_length = size + offset;

  if (new_length > inode->data.length){
    lock_acquire(&extend_lock);
    lock_release(&extend_lock);
  }
  while (size > 0) 
    {
      /* Disk sector to read, starting byte offset within sector. */
      lock_acquire(&extend_lock);
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      lock_release(&extend_lock);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      if ((int)sector_idx < 0){
          if (bounce != NULL) 
  free (bounce);
        return 0;
      }
      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0){
        break;
      }
      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Read full sector directly into caller's buffer. */
      if ((int)sector_idx < 0){
          if (bounce != NULL) 
  free (bounce);
        return 0;
      }
          block_read (fs_device, sector_idx, buffer + bytes_read);
        }
      else 
        {
          /* Read sector into bounce buffer, then partially copy
             into caller's buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL){
//                 PANIC("TRUE TERROR");
                break;
              }
            }
      if ((int)sector_idx < 0){
          if (bounce != NULL) 
  free (bounce);
        return 0;
      }
            block_read (fs_device, sector_idx, bounce);
          memcpy (buffer + bytes_read, bounce + sector_ofs, chunk_size);
        }
      
      /* Advance. */
//       printf("Chunk size is: %d\n", chunk_size);
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  free (bounce);

  return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset) 
{
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;
  uint8_t *bounce = NULL;
  
  if (inode->deny_write_cnt){
    DEBUG("Write right denied");
    return 0;
  }
  block_sector_t length_in_sectors = bytes_to_sectors(inode->data.length);
  block_sector_t sectors_needed = bytes_to_sectors(offset + size);
  block_sector_t num_sectors = sectors_needed - length_in_sectors;
  off_t new_length = offset + size;
//   printf ("The length in sectors is %d\n", length_in_sectors);
//   printf ("The length we need is %d\n", sectors_needed);
  if (num_sectors > 0){
      lock_acquire(&extend_lock);
      ASSERT(inode_extend_file (inode, sectors_needed, new_length));
      lock_release(&extend_lock);
//    printf("Size = %d\n", size);
//       PANIC("Extending?!");
  }
      if(new_length > inode->data.new_length){
        inode->data.new_length = new_length;
//         block_write (fs_device, inode->data.inode_disk_number, &inode->data);
      }
  while (size > 0) 
    {
      /* Sector to write, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode->data.new_length - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually write into this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0){
        break;
      }
      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Write full sector directly to disk. */
          block_write (fs_device, sector_idx, buffer + bytes_written);
        }
      else 
        {
          /* We need a bounce buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }

          /* If the sector contains data before or after the chunk
             we're writing, then we need to read in the sector
             first.  Otherwise we start with a sector of all zeros. */
          if (sector_ofs > 0 || chunk_size < sector_left) 
            block_read (fs_device, sector_idx, bounce);
          else
            memset (bounce, 0, BLOCK_SECTOR_SIZE);
          memcpy (bounce + sector_ofs, buffer + bytes_written, chunk_size);
          block_write (fs_device, sector_idx, bounce);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }
  free (bounce);
  int i = 0;
      if(new_length > inode->data.length){
        inode->data.length = new_length;
        block_write (fs_device, inode->data.inode_disk_number, &inode->data);
      }
  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode) 
{
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode) 
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  return inode->data.length;
}
