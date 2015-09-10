#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>
#include <debug.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "filesys/off_t.h"
#include "vm/frame.h"

struct page_table_entry
{
    bool resident;
    bool swapped;
    off_t file_offset;
    size_t page_read_bytes;
    size_t page_zero_bytes;
    bool writable;
    void *paddr;
    struct frame_table_entry *fte;
    struct hash_elem hash_elem;
    struct lock lock;
};

unsigned page_hash (const struct hash_elem *, void *aux UNUSED);
bool page_less (const struct hash_elem *, const struct hash_elem *, void *aux UNUSED);
void page_init (void);
struct page_table_entry *page_lookup (struct thread *, const void *);
bool set_supp_page_table_entry (void *, off_t, size_t, size_t, bool);
bool install_virtual_page(void *, struct frame_table_entry *);
void free_page (struct hash_elem *, void *);
void free_page_table (void);
struct lock *get_page_lock (void);
#endif
