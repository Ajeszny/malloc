#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "custom_unistd.h"

enum pointer_type_t
{
    pointer_null,
    pointer_heap_corrupted,
    pointer_control_block,
    pointer_inside_fences,
    pointer_inside_data_block,
    pointer_unallocated,
    pointer_valid
};


int heap_setup(void);

void heap_clean(void);

void* heap_malloc(size_t size);
void* heap_calloc(size_t number, size_t size);
void* heap_realloc(void* memblock, size_t count);
void* heap_malloc_aligned(size_t count);
void* heap_calloc_aligned(size_t number, size_t size);
void* heap_realloc_aligned(void* memblock, size_t size);
void *heap_realloc_d(void *memblock, size_t count);
void  heap_free(void* memblock);

size_t heap_get_largest_used_block_size(void);
enum pointer_type_t get_pointer_type(const void* const pointer);

int heap_validate(void);
