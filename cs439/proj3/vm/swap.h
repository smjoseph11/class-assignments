#ifndef VM_SWAP_H
#define VM_SWAP_H
#include "devices/block.h"
#include "threads/palloc.h"
#include <stdbool.h>
#include <stdint.h>
#include "threads/thread.h"

struct swap_table_entry
{
  int sd;
  bool allocated;
  void *fte_paddr;
  struct thread *owner;
};

struct swap_table_entry* get_free_swap(void);
void write_to_swap(void *, int);
void read_from_swap(void *, int);
int find_swap_sd (void *paddr);
#endif
