#ifndef SLABS_H__
#define  SLABS_H__

#include <libc.h>
#include <list.h>

struct slab_t {
    
    int count;
    int left;
    char *mem;
    unsigned int *freeSpaces;
    unsigned int *occupiedSpaces;

};

void init_slab(struct slab_t *slab, unsigned int elem_size, unsigned int mem_size);
int detroy_slab (struct slab_t *slab);
int deallocate_mem(struct slab_t *slab, char *addr);
char *allocate_mem(struct slab_t *slab);


#endif
