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

list_head_chunk* my_push_hook(list_head_chunk* list, chunk_header* data);
list_head_chunk* my_pop_hook(list_head_chunk* list);

/* leave possibilty open, to allocate heaps for each vm,
 * (representing threads), but won't be implemented for the
 * time being...
 */
void allocate_heap(sinavm_data* vm, size_t size)
{
	/* set up dummy hooks for push/pop to test them */
    sinavm_push_front_hook = my_push_hook;
    sinavm_pop_front_hook = my_pop_hook;
	sinavm_push_back_hook = my_push_hook;
}

void* allocate_chunk(int type)
{
	void* result = malloc(sizeof_chunk(type));
	
	chunk_header* header = (chunk_header*) result;
	header->type = type;
	header->colour = 0; /* doesn't matter for this allocator */
	
	return result;
}

list_head_chunk* my_push_hook(list_head_chunk* list, chunk_header* data)
{
    printf("In my_push_hook\n");
	return list;
}

list_head_chunk* my_pop_hook(list_head_chunk* list)
{
    printf("In my_pop_hook\n");
	return list;
}
