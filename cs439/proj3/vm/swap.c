#include "vm/swap.h"
#include "threads/synch.h"
#include "threads/palloc.h"
#include "devices/block.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include <string.h>
#include <stdio.h>


struct swap_table_entry *
get_free_swap (void) {
  struct thread *t = thread_current();
  struct swap_table_entry *swap_table = get_swap_table ();
  struct swap_table_entry *empty_swap = NULL;
  int i;
  for (i = 0; i < SWAP_TABLE_SIZE; i++){
    if (!swap_table[i].allocated) {

      empty_swap = &swap_table[i];
      empty_swap->allocated = true;
      empty_swap->owner = t;
      empty_swap->fte_paddr = NULL;
      ASSERT(empty_swap != NULL);
      return empty_swap;
    }
  }
  
  /*Control should never reach. Out of swap. */
  ASSERT(false);
}

/* Only call these if you're sure you want to overwrite your frame */
void 
write_to_swap (void *kpage, int sd) {

  struct block *swap = get_swap_space();  

//   void *tmp = kpage;
  int i;
  void *buff = kpage;
  for (i = 0; i < SEC_PER_PAGE; i++){
    block_write(swap, sd * SEC_PER_PAGE + i, buff);
    buff += BLOCK_SECTOR_SIZE;
  }
//   hex_dump (kpage, kpage, 4096, true);
//   hex_dump (buff, buff, 4096, true);
}


void 
read_from_swap (void *kpage, int sd) {
  struct block *swap = get_swap_space();  

  int i;
  void *buff = kpage;
  for (i = 0; i < SEC_PER_PAGE; i++){
    block_read(swap, sd * SEC_PER_PAGE + i, buff);
    buff += BLOCK_SECTOR_SIZE;
  }
}

int
find_swap_sd (void *paddr) {
  int i;
  struct swap_table_entry **swap_table = get_swap_table ();
  for (i = 0; i < SWAP_TABLE_SIZE; i++){
    if (swap_table[i] != NULL) {
      if (swap_table[i]->fte_paddr == paddr) {
        return i;
      }
    }
  }

  return -1;
}
