#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    int result = init_heap(4096); // restult=0 means no errors creating heap
    assert(result==0); // assert means quit program if condition is not true

    // display the list of free and allocated block at the starts	
    disp_heap();

    void *p1 = balloc(400);
    void *p2 = balloc(400);
    void *p3 = balloc(400);
    void *p4 = balloc(400);
    void *p5 = balloc(400);
    void *p6 = balloc(400);
    void *p7 = balloc(400);
    void *p8 = balloc(400);
    assert(p1 != NULL);
    assert(p2 != NULL);
    assert(p3 != NULL);
    assert(p4 != NULL);
    assert(p5 != NULL);
    assert(p6 != NULL);
    assert(p7 != NULL);
    assert(p8 != NULL);

    assert(bfree(p2) == 0);
    assert(bfree(p1) == 0);
    assert(bfree(p3) == 0);

    assert(bfree(p6) == 0);
    assert(bfree(p8) == 0);
    assert(bfree(p7) == 0);

    assert(bfree(p5) == 0);
    assert(bfree(p4) == 0);

    void *pbig = balloc(4000);
    assert(pbig != NULL);

    exit(0);
}
