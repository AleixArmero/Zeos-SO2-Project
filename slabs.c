#include <slabs.h>

#define PAGESIZE 4096
#define MAXCHUNKS 32

struct mem_chunk mems[MAXCHUNKS];

// Initializes the total amount of available memory chunks
// First use this function
void init_chunks (struct free_chunks *f)
{
  INIT_LIST_HEAD (&f->freelist);
  for (int i = 0; i < MAXCHUNKS; i++)
    list_add_tail(&(mems[i].anchor), &f->freelist);
  f->size = MAXCHUNKS;
}

// Creates an slab allocator and return the number of chunks available
// Returns -1 if error
int create_slab(struct free_chunks *f, struct slab_t *slab, unsigned int elem_size, unsigned int mem_size) {

    int n = mem_size/elem_size;
    if (n > f->size)
      return -1;
    
    INIT_LIST_HEAD(&slab->freemem);
    INIT_LIST_HEAD(&slab->filledmem);

    char *mem = memRegGet ((mem_size/PAGESIZE)+1);
    if (mem == (char *) 0)
    	return -1;
    slab->mem = mem;

    for (int i = 0; i < n; i++){
      struct list_head *l = list_first (&f->freelist);
      struct mem_chunk *m = list_entry (l, struct mem_chunk, anchor);
      m->mem_pos = (char *) (((unsigned) mem) + elem_size * i);
      list_del (l);
      list_add_tail (l, &slab->freemem);
    }

    f->size -= n;
    return n;
}

int detroy_slab (struct free_chunks *f, struct slab_t *slab) {
  int i = 0;
  struct list_head *pos, *n;
  list_for_each_safe (pos, n, &slab->freemem) {
    list_del (pos);
    list_add_tail (pos, &f->freelist);
    i++;
  }
  list_for_each_safe (pos, n, &slab->filledmem) {
    list_del (pos);
    list_add_tail (pos, &f->freelist);
    i++;
  }
  memRegDel (slab->mem);
  f->size += i;
  return 0;
}

// Deallocates a mem chunk of an slab
// Returns -1 if addr does not point to any chunk of this slab
int deallocate_mem(struct slab_t *slab, char *addr)
{
  struct list_head *pos, *n;
  list_for_each_safe (pos, n, &slab->filledmem) {
    struct mem_chunk *m = list_entry (pos, struct mem_chunk, anchor);
    if (addr == m->mem_pos) {
      list_del (&m->anchor);
      list_add_tail (&m->anchor, &slab->freemem);
      return 0;
    }
  }

  return -1; 
}

// Allocates a memory chunk
// Returns 0 if there are not chuncks available
char *allocate_mem(struct slab_t *slab)
{
  if (list_empty(&slab->freemem))
    return 0;

  struct list_head *l = list_first (&slab->freemem);
  struct mem_chunk *m = list_entry (l, struct mem_chunk, anchor);
  list_del (l);
  list_add_tail (l, &slab->filledmem);
  return m->mem_pos;
}
