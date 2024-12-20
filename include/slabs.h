#ifndef SLABS_H__
#define  SLABS_H__

#include <libc.h>
#include <list.h>

struct mem_chunk {
    char *mem_pos;
    struct list_head anchor;
};

struct slab_t {
    struct list_head freemem;
    struct list_head filledmem;
    char *mem;
};

struct free_chunks {
  struct list_head freelist;
  int size;
};

void init_chunks (struct free_chunks *f);
int create_slab (struct free_chunks *f, struct slab_t *slab, unsigned int elem_size, unsigned int mem_size);
int detroy_slab (struct free_chunks *f, struct slab_t *slab);
int deallocate_mem(struct slab_t *slab, char *addr);
char *allocate_mem(struct slab_t *slab);


#endif
