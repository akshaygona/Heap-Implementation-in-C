// allocation is too big to fit
#include <assert.h>
#include <stdlib.h>
#include "cs354heap.h"

int main() {
    assert(init_heap(4096)  == 0);
    assert(balloc(1)    != NULL);
    assert(balloc(4095) == NULL);
    exit(0);
}
