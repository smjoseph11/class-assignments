#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <stdbool.h>
#include "vm/swap.h"
struct frame_table_entry
{
    void *faddr;
    bool allocated;
    void *paddr;
    bool recent_access;
    bool pinned;
    struct thread *owner;
};

struct frame_table_entry * get_frame(bool);
void free_frame(struct frame_table_entry *);

struct frame_table_entry * evict_frame(void);
bool load_from_swap (struct swap_table_entry *);

#endif
