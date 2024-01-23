///////////////////////////////////////////////////////////////////////////////
//
// Copyright 2020-2022 Deb Deppeler based on work by Jim Skrentny
// Posting or sharing this file is prohibited, including any changes/additions.
// Fall 2023
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>

#ifndef __cs354heap_h
#define __cs354heap_h

/*
 * Define a constant that we can use as the minimum block alignment for any
 * block (in bytes).
 */
#define BLOCK_ALIGNMENT 8

int   init_heap(size_t size);
void  disp_heap();

void* balloc(size_t size);
int   bfree(void *ptr);
void  coalesce();

void* malloc(size_t size) {
    return NULL;
}

#endif // __cs354heap_h__

