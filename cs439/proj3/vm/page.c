#include <stdio.h>
#include <hash.h>
#include <debug.h>
#include "vm/page.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "threads/malloc.h"
#include "vm/swap.h"

struct lock page_lock;

void
page_init (void)
{
  lock_init (&page_lock);
}

unsigned
page_hash (const struct hash_elem *p_, void *aux UNUSED)
{
    const struct page_table_entry *p = hash_entry (p_, struct page_table_entry, hash_elem);
    return hash_bytes (&p->paddr, sizeof(p->paddr));
}

bool
page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux UNUSED)
{
    const struct page_table_entry *a = hash_entry (a_, struct page_table_entry, hash_elem);
    const struct page_table_entry *b = hash_entry (b_, struct page_table_entry, hash_elem);

    return a->paddr < b->paddr;
}

bool
set_supp_page_table_entry (void *upage, off_t ofs, size_t page_read_bytes, size_t page_zero_bytes, bool writable)
{

  struct thread *t = thread_current ();
  struct hash *page_table = &t->page_table;
  ASSERT (page_table != NULL);
//   printf ("Page table size: %d\n", hash_size (page_table));

  struct page_table_entry *pte = malloc (sizeof (struct page_table_entry));
  pte->resident = false;
  pte->swapped = false;
  pte->page_read_bytes = page_read_bytes;
  pte->page_zero_bytes = page_zero_bytes;
  pte->file_offset = ofs;
  pte->paddr = upage;
  pte->fte = NULL;
  pte->writable = writable;
  bool success = false;
  if (hash_insert (page_table, &pte->hash_elem) == NULL)
  {
    success = true;
//     printf ("Inserting page \n");
//     printf ("Thread %s\n", t->name);
//     printf ("Page table size: %d\n", hash_size (page_table));
//     printf ("paddr: %08X\n", pte->paddr);

    ASSERT (page_lookup (t, pte->paddr) != NULL);
  }
  else{
    ASSERT (false);
  }
  
  return success;
}

/* Returns the page containing the given virtual address,
   or a null pointer if no such page exists. */
struct page_table_entry *
page_lookup (struct thread *t, const void *address)
{

  struct page_table_entry p;
  struct hash_elem *e;

  p.paddr = (void *)address;

  e = hash_find (&t->page_table, &p.hash_elem);
  
  if (e == NULL) {
    return NULL;
  }

  return hash_entry (e, struct page_table_entry, hash_elem);
}

void
free_page (struct hash_elem *e, void *aux UNUSED)
{
  struct page_table_entry *pte = hash_entry (e, struct page_table_entry, hash_elem);

  if (pte != NULL) {
    if (pte->resident){
      if (pte->swapped) {
        pte->ste->allocated = false;
        pte->ste->fte_paddr = NULL;
        pte->ste->owner = NULL;
      } else if (pte->fte != NULL) {
        free_frame(pte->fte);
        pte->fte = NULL;
      }
    }
    free(pte); 
  }

}

void
free_page_table (void)
{
  struct thread *t = thread_current ();

  hash_destroy (&t->page_table, free_page);
}

