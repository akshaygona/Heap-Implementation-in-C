#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cs354heap.h"


#define MIN_PAYLOAD_SIZE (round_up_block_size(sizeof(BlockFooter)))
#define MIN_BLOCK_SIZE (sizeof(BlockHeader) + MIN_PAYLOAD_SIZE)
#define P_BITMASK (1<<0)
#define A_BITMASK (1<<1)
#define SIZE_BITMASK 0xFFFFFFF8
#define ABIT_FREE 0
#define ABIT_USED A_BITMASK
#define PBIT_FREE 0
#define PBIT_USED P_BITMASK
#define END_MARK_MAGIC (0xDEADFFFC | ABIT_USED)
typedef struct BlockHeader {
    size_t size_status;
    size_t padding;
} BlockHeader;

typedef struct BlockFooter {
    struct BlockFooter *free_list_prev;
    struct BlockFooter *free_list_next;
    size_t size;
} BlockFooter;

BlockHeader *heap_start = NULL;

size_t heap_size = 0xFFFFFFFF;

BlockFooter *free_list = NULL;

static int is_end_mark(BlockHeader *header) {
    return (header->size_status & ~P_BITMASK) == END_MARK_MAGIC;
}

static void set_block_header(BlockHeader *header, size_t size, int abit, int pbit) {
    assert((size % 8) == 0);
    assert((abit & ~A_BITMASK) == 0);
    assert((pbit & ~P_BITMASK) == 0);
    header->size_status = size | abit | pbit;
}

static size_t get_block_size(BlockHeader *header) {
    return header->size_status & SIZE_BITMASK;
}


static int get_block_abit(BlockHeader *header) {
    return header->size_status & A_BITMASK;
}

static void set_block_abit(BlockHeader *header, int abit) {
    header->size_status &= ~A_BITMASK;
    header->size_status |= abit;
}

static int get_block_pbit(BlockHeader *header) {
    return header->size_status & P_BITMASK;
}

static void set_block_pbit(BlockHeader *header, int pbit) {
    header->size_status &= ~P_BITMASK;
    header->size_status |= pbit;
}

static void *get_block_payload(BlockHeader *header) {
    return (void *)((char *)header + sizeof(BlockHeader));
}

/*
 *
 *-----------
 *| header1 |
 *-----------
 *| payload |
 *||
 *|...|
 *-----------
 *| header2 |
 *-----------
 *| payload |
 *||
 *|...|
 *-----------
 */
static BlockHeader *get_next_adjacent_block(BlockHeader *header) {
    return (BlockHeader *)((char *)header + get_block_size(header) + sizeof(BlockHeader));
}


static BlockHeader *get_header_from_payload(void *payload) {
    return (BlockHeader *)((char *)payload - sizeof(BlockHeader));
}

static BlockFooter *find_block_footer(BlockHeader *header) {
    return (BlockFooter *)((char *)header + sizeof(BlockHeader) + get_block_size(header) - sizeof(BlockFooter));
}

static BlockHeader *find_block_header(BlockFooter *footer) {
    if (footer == NULL){
        return NULL;
        }
    return (BlockHeader *)((char *)footer + sizeof(BlockFooter) - (footer -> size) - sizeof(BlockHeader));
}

static BlockHeader *get_prev_adjacent_block(BlockHeader *header) {
    if (get_block_pbit(header) == PBIT_FREE){
        BlockFooter *footer = (BlockFooter *)header - 1;
        return find_block_header(footer);
        }
    return NULL;
}


static void set_block_footer_size(BlockFooter *footer, size_t size) {
    footer->size = size;
}

static BlockHeader *best_fit_select_block(size_t size) {
    BlockFooter *cur = free_list; //start at first block
    BlockFooter *best = NULL; //best block
    size_t min_diff = SIZE_MAX; //min difference between size and block size
    while (cur != NULL){ //iterate through free list
        size_t cur_size = cur->size; //get size of current block
        if ((cur_size >= size) && ((cur_size - size)< min_diff)) { //if current block is big enough and difference is smaller than min difference
            best = cur; //set best to current block
            min_diff = cur_size - size; //set min difference to difference between current block and size
            }
        cur = cur -> free_list_next; //move to next block
        }
    return find_block_header(best); //return pointer to header of best block
}

static size_t round_up_block_size(size_t unrounded) {
    size_t almost_too_far = unrounded + BLOCK_ALIGNMENT - 1;
    size_t remainder = almost_too_far % BLOCK_ALIGNMENT;
    return almost_too_far - remainder;
}

static void remove_from_free_list(BlockHeader *header) {
    BlockFooter *footer = find_block_footer(header); //find footer
    BlockFooter *prev = footer->free_list_prev; //get previous block
    BlockFooter *next = footer->free_list_next; //get next block
    if (prev != NULL) { //if there is a previous block
        prev->free_list_next = next; //set previous block's next block to next block
        } else { //if there is no previous block
        free_list = next; //set next block as head
        }  
    if (next != NULL) { //if there is a next block
        next->free_list_prev = prev; //set next block's previous block to previous block
        }

}
static void add_to_free_list(BlockHeader *header) {
    BlockFooter *footer = find_block_footer(header); //find footer
    footer->free_list_prev = NULL; //set previous block to null
    footer->free_list_next = free_list; //set next block to head
    if (free_list != NULL) { //if there is a head
        free_list->free_list_prev = footer; //set head's previous block to new block
        }
    free_list = footer; //set new block as head
}

static void make_block_used(BlockHeader *header) {
    set_block_abit(header, ABIT_USED); //set A-bit to used
    BlockHeader *next = get_next_adjacent_block(header); //get next block
    if ((next != NULL) && !is_end_mark(next)) { //if next block is not null and not end mark
        set_block_pbit(next, PBIT_USED); //set P-bit to used
        }
    remove_from_free_list(header); //remove block from free list

}

static void make_block_free(BlockHeader *header) {
    set_block_abit(header, ABIT_FREE); //set A-bit to free
    size_t size = get_block_size(header); //get block size
    BlockFooter *footer = find_block_footer(header); //find footer
    set_block_footer_size(footer, size); //set footer size to block size
    BlockHeader *next = get_next_adjacent_block(header); //get next block
    if ((next != NULL) && !is_end_mark(next)) { //if next block is not null and not end mark
        set_block_pbit(next, PBIT_FREE); //set P-bit to free
        }
    add_to_free_list(header); //add block to free list

}

static int should_split(BlockHeader *header, size_t desired_size) {
    size_t min_split_size = desired_size + MIN_BLOCK_SIZE; //get minimum split size
    size_t total_block_size = get_block_size(header); //get total block size
    return (total_block_size >= min_split_size); //return true if total block size is greater than or equal to minimum split size
}

static BlockHeader *split_block(BlockHeader *header, size_t desired_size) {
    remove_from_free_list(header); //remove block from free list
    size_t size = (desired_size); //get allocated block size
    if (!should_split(header, size)) { //if block cannot be split
        return NULL;
        }
    size_t remainder_size = get_block_size(header) - size - sizeof(BlockHeader); //get remainder block size
    BlockHeader *block = header;
    set_block_header(block, size, ABIT_USED, get_block_pbit(header)); //set allocated block header
    //printf("allocated block size: %zu\n", size);
    set_block_footer_size(find_block_footer(block), size); //set allocated block footer
    add_to_free_list(block); //add allocated block to free list
    header = (BlockHeader *)((char *)header + desired_size + sizeof(BlockHeader)); //set header to remainder block
    set_block_header(header, remainder_size, ABIT_FREE, PBIT_FREE); //set remainder block header
    //printf("remainder block size: %zu\n", remainder_size);
    set_block_footer_size(find_block_footer(header), remainder_size); //set remainder block footer
    add_to_free_list(header); //add remainder block to free list
    return header; //return pointer to new remainder block
}

static void coalesce_with_next_block(BlockHeader *header) {
    BlockHeader *next = get_next_adjacent_block(header); //get next block
    if (!is_end_mark(next) && get_block_abit(next) == ABIT_FREE) { //if next block is not end mark and is free
        remove_from_free_list(header); //remove block from free list
        remove_from_free_list(next); //remove next block from free list
        //printf("coalesced block size: %zu\n", get_block_size(header) + get_block_size(next) + sizeof(BlockHeader));
        size_t size = get_block_size(header) + get_block_size(next) + sizeof(BlockHeader); //get total size of coalesced block
        set_block_header(header, size, ABIT_FREE, get_block_pbit(header)); //set header of coalesced block
        set_block_footer_size(find_block_footer(header), size); //set footer of coalesced block
        //printf("coalesced block size: %zu\n", get_block_size(header));
        BlockHeader *next = get_next_adjacent_block(header); //get next block
        if (next != NULL){ //if next block is not null
            set_block_pbit(next, PBIT_FREE); //set P-bit of next block to free
            }
        add_to_free_list(header); //add coalesced block to free list
        }
}

void* balloc(size_t size){
    if (size < 1) { //if size is less than 1
        return NULL;
        }
    size_t size1 = round_up_block_size(size); //round up block size
    BlockHeader *best_block = best_fit_select_block(size1); //get best fit block
    if (best_block == NULL) { //if no best fit block
        return NULL;
        }
    split_block(best_block, size1); //split block
    make_block_used(best_block); //make block used
    return get_block_payload(best_block); //return pointer to payload
}

int bfree(void *ptr) {
    if (ptr == NULL) { //if pointer is null
        return -1;
        }
    if ((size_t)ptr % 8 != 0) { //if pointer is not a multiple of 8
        return -1;
        }
    BlockHeader *header = get_header_from_payload(ptr); //get header from payload
    if (header == NULL){ //if header is null
        return -1;
        }
    if ((header < heap_start) || ((char*)header >= ((char *)heap_start + heap_size))) { //if header is outside of heap space
        return -1;
        }
    if (get_block_abit(header) == ABIT_FREE) { //if block is already free
        return -1;
        }

    //printf("problem aafashflas");
    make_block_free(header); //make block free
    coalesce_with_next_block(header); //coalesce with next block
    BlockHeader *prev_block = get_prev_adjacent_block(header); //get previous block
    if(prev_block != NULL){
        coalesce_with_next_block(prev_block); //coalesce with previous block
        }
    return 0;
}

int init_heap(size_t size) {

    static int allocated_once = 0;
    if (allocated_once != 0) {
        fprintf(stderr,
        "Error: init_heap has allocated space during a previous call\n");
        return -1;
        }
    int pagesize = getpagesize();

    if ((size < 1) || ((size % pagesize) != 0)) {
        fprintf(stderr, "Error: Requested block size is not a positive"
        " multiple of page size.\n");
        return -1;
        }
    void *mmap_ptr = mmap(NULL, size + 2 * pagesize,
    PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (MAP_FAILED == mmap_ptr) {
        fprintf(stderr, "Error: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
        }

    allocated_once = 1;
    heap_size = size;
    void *start_guard_page = mmap_ptr;
    void *end_guard_page = ((char*)mmap_ptr) + pagesize + size;

    mprotect(start_guard_page, pagesize, PROT_NONE);
    mprotect(end_guard_page, pagesize, PROT_NONE);
    BlockHeader *end_mark = (BlockHeader*)((char*)end_guard_page - MIN_BLOCK_SIZE);
    end_mark->size_status = END_MARK_MAGIC;
    heap_start = (BlockHeader*)(((char*)start_guard_page) + pagesize);
    set_block_header(heap_start,
    heap_size - MIN_BLOCK_SIZE - sizeof(BlockHeader),
    ABIT_FREE, PBIT_USED);
    make_block_free(heap_start);

    return 0;
}

void disp_heap() {
    BlockHeader *current = heap_start;
    BlockHeader *next_block;

    size_t counter = 1;
    size_t used_size =0;
    size_t free_size =0;
    int is_used= -1;
    int is_p_used = -1;
    size_t size = 0;

    fprintf(stdout, "Heap Start: %p\n", heap_start);
    fprintf(stdout, "Heap Size: %d\n", heap_size);

    fprintf(stdout,
    "*********************************** HEAP: Block List ****************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tBegin\t\tEnd\t\tSize\n");
    fprintf(stdout,
    "---------------------------------------------------------------------------------\n");

    while (!is_end_mark(current)) {
        is_used = get_block_abit(current) == ABIT_USED;
        is_p_used = get_block_pbit(current) == PBIT_USED;
        next_block = get_next_adjacent_block(current);
        size = get_block_size(current);

        if (is_used)
        used_size += size;
        else
        free_size += size;

        fprintf(stdout, "%u\t%s\t%s\t0x%08lx\t0x%08lx\t%4u\n",
        counter, is_used ? "alloc" : "FREE",
        is_p_used ? "alloc" : "FREE",
        (unsigned long)current,
        (unsigned long)next_block,
        get_block_size(current));

        current = next_block;
        counter += 1;
        }

    fprintf(stdout,
    "---------------------------------------------------------------------------------\n");
    fprintf(stdout,
    "*********************************************************************************\n");
    fprintf(stdout, "Total used size = %4u\n", used_size);
    fprintf(stdout, "Total free size = %4u\n", free_size);
    fprintf(stdout, "Total size= %4u\n", used_size + free_size);
    fprintf(stdout,
    "*********************************************************************************\n");
    fflush(stdout);

    return;
}