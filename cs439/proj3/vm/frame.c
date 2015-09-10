#include "vm/frame.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "threads/palloc.h"
#include <debug.h>
#include <string.h>
#include <random.h>
#include <stdio.h>
#include "threads/thread.h"
#include "vm/swap.h"
#include "userprog/pagedir.h"
#include "threads/malloc.h"


int frame_table_index = 0;
/* Returns a free frame from the frame table and set it to be allocated. */
struct frame_table_entry *
get_frame (bool zero) {
  ASSERT(lock_held_by_current_thread(get_frame_lock()));
  struct thread *t = thread_current();
  struct frame_table_entry *frame_table = get_frame_table ();
  struct frame_table_entry *empty_frame = NULL;
  int i;
  for (i = 0; i < get_frame_table_size (); i++){
    if (!frame_table[i].allocated) {
      if (zero) {
        memset(frame_table[i].faddr, 0x0, PGSIZE);
      }

      empty_frame = &frame_table[i];
      empty_frame->allocated = true;
      empty_frame->owner = t;
      ASSERT(empty_frame != NULL);
      return empty_frame;
    }
  }
  /* Memory is full, evict */
  empty_frame = evict_frame ();
  empty_frame->allocated = true;
  empty_frame->owner = t;
  ASSERT(empty_frame != NULL);
  return empty_frame;

}

void
free_frame(struct frame_table_entry *fte) {
  ASSERT(lock_held_by_current_thread(get_frame_lock()));
  ASSERT (pg_ofs (fte->faddr) == 0);
  ASSERT (fte->faddr != NULL);
  ASSERT (!fte->pinned);

  fte->allocated = false;
  fte->paddr = NULL;
  fte->recent_access = false;
  fte->owner = NULL;
  memset(fte->faddr, 0xcc, PGSIZE);
//    memset(fte->faddr, 0x0, PGSIZE);
}

struct frame_table_entry *
evict_frame (void) {
  ASSERT(lock_held_by_current_thread(get_frame_lock()));
  if (DEBUG) {
    printf("Evicting Frame\n");
  }
  struct frame_table_entry *frame_table = get_frame_table ();
  ASSERT (frame_table != NULL);

  /* Clock eviction */
  struct frame_table_entry *fte;
  while (true) {
    fte = &frame_table[frame_table_index++];
    ASSERT(fte != NULL);

    if (frame_table_index >= get_frame_table_size()){
      frame_table_index = 0;
    }

    uint32_t *pd = fte->owner->pagedir;
    bool accessed = pagedir_is_accessed (pd, fte->paddr);

    if (accessed) {
      pagedir_set_accessed (pd, fte->paddr, false);
    } else {
      break;
    }
  }

  /* check if null */
  ASSERT(fte != NULL);
  ASSERT(fte->allocated);
  ASSERT(fte->owner != NULL);
  ASSERT(fte->paddr != NULL);
  ASSERT(!fte->pinned);

  pagedir_clear_page(fte->owner->pagedir, fte->paddr);
  struct page_table_entry *pte = page_lookup(fte->owner, fte->paddr);

  /* Don't write page to swap if it is read-only */
//   if (pte->writable && pagedir_is_dirty(fte->owner->pagedir, fte->paddr)) {
  if (pte->writable) {
      struct swap_table_entry *ste = get_free_swap ();
      ASSERT(ste != NULL);
      ste->fte_paddr = fte->paddr;
      write_to_swap(fte->faddr, ste->sd);
      pte->swapped = true;
      pte->ste = ste;
      ste->owner = fte->owner;
  }
  else {
  pte->resident = false;
  }
  free_frame(fte);
  pte->fte = NULL; 
  return fte;
}

  bool
load_from_swap(struct swap_table_entry *ste)
{
  ASSERT(lock_held_by_current_thread(get_frame_lock()));
  if (DEBUG) {
    printf ("loading from swap\n");
  }

  struct thread *t = thread_current ();
  struct frame_table_entry *fte = get_frame(false);

  ASSERT(fte != NULL);
  ASSERT(fte->allocated);
  ASSERT(fte->owner != NULL);

  int sd = ste->sd; 
  ASSERT (ste->fte_paddr!=NULL);
  ASSERT (ste->owner == t);

  fte->paddr = ste->fte_paddr;
  fte->owner = t;

  read_from_swap (fte->faddr,sd);
  ste->allocated = false;
  ste->fte_paddr = NULL;
  ste->owner = NULL;
  struct page_table_entry *pte = page_lookup (t, fte->paddr);

  if (pte == NULL) {
    free_frame(fte);
    return false;
  }

  pagedir_set_page(active_pd(), pte->paddr, fte->faddr, true); 

  pte->swapped = false;
  pte->ste = NULL;
  pte->fte = fte;
  ASSERT(fte->faddr != NULL);
  return true;
}
