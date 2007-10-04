/*
 * simple_allocator.c
 *
 * brain dead implementation of allocator.h: just use malloc.
 * we will use this to make sure the parser and interpreter
 * are working properly before implementing the various
 * garbage collectors.
 */

#include "sina_types.h"
#include <stdlib.h>
#include "sina_error.h"
#include "sinavm.h"

/* leave possibilty open, to allocate heaps for each vm,
 * (representing threads), but won't be implemented for the
 * time being...
 */
void allocate_heap(sinavm_data* vm, size_t size)
{
	/* do nothing */
}

void* allocate_chunk(int type)
{
	void* result = malloc(sizeof_chunk(type));
	
	chunk_header* header = (chunk_header*) result;
	header->type = type;
	header->colour = 0; /* doesn't matter for this allocator */
	
	return result;
}
