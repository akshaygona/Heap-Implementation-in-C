#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    int result = init_heap(4096); // restult=0 means no errors creating heap
    assert(result==0); // assert means quit program if condition is not true

    void *p1 = balloc(4000);
    void *p2 = balloc(4000);
    assert(p1 != NULL);
    assert(p2 == NULL);

    bfree(p1);

    void *p3 = balloc(4000);
    assert(p3 != NULL);

    // display the list of free and allocated block at the starts	
    disp_heap();

    exit(0);
}
