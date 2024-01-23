#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    int result = init_heap(4096); // restult=0 means no errors creating heap
    assert(result==0); // assert means quit program if condition is not true

    // display the list of free and allocated block at the starts	
    disp_heap();

    void *p1 = balloc(2000);
    void *p2 = balloc(2000);
    assert(p1 != NULL);
    assert(p2 != NULL);

    bfree(p1);
    bfree(p2);

    void *p3 = balloc(4000);
    assert(p3 != NULL);

    exit(0);
}
