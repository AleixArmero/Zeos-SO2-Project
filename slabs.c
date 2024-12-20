#include <slabs.h>

#define PAGESIZE 4096

void deleteOccupiedSpace(int pos, struct slab_t *slab) {

  unsigned int mem_pos = slab->occupiedSpaces[pos];
  slab->freeSpaces[slab->left] = mem_pos;
  slab->left++;


  slab->count--;

  for (int i = pos; i < slab->count-1; i++) {
    slab->occupiedSpaces[i] = slab->occupiedSpaces[i+1];
  }

}

// Creates an slab allocator and return the number of chunks available
// Returns -1 if error
void init_slab(struct slab_t *slab, unsigned int elem_size, unsigned int mem_size) {

  int elem_amount = elem_size/mem_size;

  slab->left = elem_amount;
  slab->count = 0;
  char *mem = memRegGet(mem_size/PAGESIZE + 1);
  slab->mem = mem;

  slab->freeSpaces = (unsigned int*) memRegGet(elem_amount*sizeof(unsigned int)/PAGESIZE + 1);
  slab->occupiedSpaces = (unsigned int*) memRegGet(elem_amount*sizeof(unsigned int)/PAGESIZE + 1);

  slab->freeSpaces[0] = (unsigned int) mem;

  for (int i = 1; i < elem_amount; i++) {
    slab->freeSpaces[i] = slab->freeSpaces[i-1] + i*elem_size;
  }

}

int detroy_slab (struct slab_t *slab) {
  memRegDel((char *)slab->freeSpaces);
  memRegDel((char *)slab->occupiedSpaces);
  memRegDel(slab->mem);
  return 1;
}

// Deallocates a mem chunk of an slab
// Returns -1 if addr does not point to any chunk of this slab
int deallocate_mem(struct slab_t *slab, char *addr)
{
  for (int i = 0; i < slab->count; i++) {
    if (slab->occupiedSpaces[i] == (unsigned int) addr) {
      deleteOccupiedSpace(i, slab);
      return 0;
    }
  }
  return -1;
}

// Allocates a memory chunk
// Returns 0 if there are not chuncks available
char *allocate_mem(struct slab_t *slab)
{
  if (slab->left == 0) return 0;

  slab->left--;

  char *return_mem = (char *) slab->freeSpaces[slab->left];

  slab->occupiedSpaces[slab->count] = slab->freeSpaces[slab->left];

  slab->count++;

  return return_mem;

}
