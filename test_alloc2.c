// a few allocations in multiples of 4 bytes
#include <assert.h>
#include <stdlib.h>
#include "cs354heap.h"

int main() {
   assert(init_heap(4096) == 0);
   assert(balloc(4) != NULL);
   assert(balloc(8) != NULL);
   assert(balloc(16) != NULL);
   assert(balloc(24) != NULL);
   exit(0);
}
