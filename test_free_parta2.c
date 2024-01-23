#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    int result = init_heap(4096); // restult=0 means no errors creating heap
    assert(result==0); // assert means quit program if condition is not true
            
    void *p0 = balloc(400);
    void *p1 = balloc(400);
    void *p2 = balloc(400);
    void *p3 = balloc(400);
    void *p4 = balloc(400);
    void *p5 = balloc(400);
    void *p6 = balloc(400);
    void *p7 = balloc(400);
    void *p8 = balloc(400);
    void *p9 = balloc(400);
    assert(p0 != NULL);
    assert(p1 != NULL);
    assert(p2 != NULL);
    assert(p3 != NULL);
    assert(p4 != NULL);
    assert(p5 != NULL);
    assert(p6 != NULL);
    assert(p7 != NULL);
    assert(p8 != NULL);

    // Free every other allocation
    bfree(p0);
    bfree(p2);
    bfree(p4);
    bfree(p6);
    bfree(p8);

    // Try allocating again. Check that we get back one of the same blocks again.
    void *p10 = balloc(400);
    void *p11 = balloc(400);
    void *p12 = balloc(400);
    void *p13 = balloc(400);
    void *p14 = balloc(400);
    assert(p10 != NULL);
    assert(p11 != NULL);
    assert(p12 != NULL);
    assert(p13 != NULL);
    assert(p14 != NULL);

    int *p1s[5] = {p0, p2, p4, p6, p8};
    int *p2s[5] = {p10, p11, p12, p13, p14};

    for(int i = 0; i < 5 ; ++i) {
        int found = 0; // FALSE
        for(int j = 0; j < 5 ; ++j) {
            if (p2s[i] == p1s[j]) {
                found = 1; // TRUE
                break;
            }
        }
        assert(found);
    }

    // display the list of free and allocated block at the starts	
    disp_heap();

    exit(0);
}
