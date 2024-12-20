#include <slabs.h>

#define PAGESIZE 4096

int roundUpDiv(float a, float b) {

    float div = a/b;
    int enter = div;
    if (div - enter > 0) return enter+1;

}

struct slab_t create_slab(unsigned int elem_size, unsigned int mem_size) {

    struct slab_t slab;

    INIT_LIST_HEAD(&slab.freemem);
    INIT_LIST_HEAD(&slab.filled);

    char *mem = memRegGet(roundUpDiv(mem_size/PAGESIZE));
    unsigned int lastPos = mem + roundUpDiv(mem_size/PAGESIZE)*PAGESIZE;

    int n = mem_size/elem_size;

    return slab;
}

char * get_slab(struct slab_t s) {

    char *mem = s.mem + (s.count+1)*s.elem_size;

    if (mem > s.last_pos) return NULL;

    return mem;

}