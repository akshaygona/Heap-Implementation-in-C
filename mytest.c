#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include "cs354heap.h"

int main() {
    int result = init_heap(4096); // restult=0 means no errors creating heap
    assert(result==0); // assert means quit program if condition is not true
    printf("init\n");

    // display the list of free and allocated block at the starts	
    disp_heap();
    printf("disp\n");

    exit(0);
}
