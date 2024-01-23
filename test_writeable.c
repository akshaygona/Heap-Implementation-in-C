// write to a block of allocated heap and check the value
#include <assert.h>
#include <stdlib.h>
#include "cs354heap.h"

int main() {
   assert(init_heap(4096) == 0);
   int* ptr = (int*) balloc(sizeof(int));
   assert(ptr != NULL);
   *ptr = 42;   // check pointer is in a writeable page
   assert(*ptr == 42);
   exit(0);
}
