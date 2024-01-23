// a few allocations in multiples of 4 bytes checked for alignment
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    assert(init_heap(4096) == 0);
    int* ptr[4];

    ptr[0] = (int*) balloc(4);
    ptr[1] = (int*) balloc(8);
    ptr[2] = (int*) balloc(16);
    ptr[3] = (int*) balloc(24);

    for (int i = 0; i < 4; i++) {
        assert(ptr[i] != NULL);
    }

    for (int i = 0; i < 4; i++) {
        assert(((int)ptr[i]) % 8 == 0);
    }

    exit(0);
}
