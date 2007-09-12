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

void allocate_heap(size_t size)
{
	/* do nothing */
}

void* allocate_chunk(int type)
{
	void* result;
	switch(type)
	{
		case INTEGER_CHUNK:
			result = malloc(sizeof(integer_chunk));
			break;

		case SYMBOL_CHUNK:
			result = malloc(sizeof(symbol_chunk));
			break;

		case ESCAPED_SYMBOL_CHUNK:
			result = malloc(sizeof(escaped_symbol_chunk));
			break;

		case LIST_NODE_CHUNK:
			result = malloc(sizeof(list_node_chunk));
			break;

		case LIST_HEAD_CHUNK:
			result = malloc(sizeof(list_head_chunk));
			break;

		case BLOCK_CHUNK:
			result = malloc(sizeof(block_chunk));
			break;
	}

	if (NULL == result)
	{
		/* FIXME: die a horrible death */
	}
	else
	{
		chunk_header* header = (chunk_header*) result;
		header->type = type;
		header->colour = 0; /* doesn't matter for this allocator */
	}
	return result;
}
