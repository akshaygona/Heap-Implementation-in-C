// a simple 8 byte allocation
#include <assert.h>
#include <stdlib.h>
#include "cs354heap.h"

int main() {
    assert(init_heap(4096) == 0);
    void* ptr = balloc(8);
    assert(ptr != NULL);
    exit(0);
}
