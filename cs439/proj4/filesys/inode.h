#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include <stdbool.h>
#include <list.h>
#include "filesys/off_t.h"
#include "devices/block.h"
#include "threads/synch.h"
#define NOT_USED -1
#define MASTER_SIZE 115
#define IND_BLOCK_SIZE 128
#define IND_NUM_BLOCKS 7 
struct bitmap;

/*Indirect block. BLOCK_SECTOR_SIZE bytes big */
struct lv1_ind_block {
    block_sector_t direct_block[IND_BLOCK_SIZE];   /* Sectors of data */
};
/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
  {
    off_t length;                       /* File size in bytes */
    off_t new_length;
    block_sector_t inode_disk_number;   /* Where we are on disk */
    block_sector_t lv1_ind_block[IND_NUM_BLOCKS];       /* Sector of first level indirect block */
    block_sector_t direct_block[MASTER_SIZE];   /* Sectors of data */
    int is_dir;
    int dir_pos;
    unsigned magic;                     /* Magic number */
  };
/* In-memory inode. */
struct inode 
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    struct inode_disk data;             /* Inode content. */
    struct lock write_lock;
  };

/* DEBUG struct */
struct lv2_ind_block {
    block_sector_t lv1_ind_block[IND_BLOCK_SIZE];  /* Sectors of first level indirect blocks */
};
void inode_init (void);
bool inode_create (block_sector_t, off_t);
struct inode *inode_open (block_sector_t);
struct inode *inode_reopen (struct inode *);
block_sector_t inode_get_inumber (const struct inode *);
void inode_close (struct inode *);
void inode_remove (struct inode *);
off_t inode_read_at (struct inode *, void *, off_t size, off_t offset);
off_t inode_write_at (struct inode *, const void *, off_t size, off_t offset);
void inode_deny_write (struct inode *);
void inode_allow_write (struct inode *);
off_t inode_length (const struct inode *);
bool inode_extend_file (struct inode*, block_sector_t, off_t);
bool inode_create_dir (block_sector_t inode_disk_sector, off_t length, bool is_dir);

#endif /* filesys/inode.h */
