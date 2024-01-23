// first pointer returned is 8-byte aligned
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
   assert(init_heap(4096) == 0);
   int* ptr = (int*) balloc(sizeof(int));
   assert(ptr != NULL);
   assert(((int)ptr) % 8 == 0);
   exit(0);
}
