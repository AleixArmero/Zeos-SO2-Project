#ifndef SLABS_H__
#define  SLABS_H__

#include <libc.h>
#include <list.h>

struct mem_pos {

    char *mem_pos;
    struct list_head anchor;

};

struct slab_t {

    struct list_head freemem;
    struct list_head filled;

};

struct slab_t create_slab(unsigned int elem_size, unsigned int mem_size);


#endif