/*
 * twospace_allocator.c - implements the TwoSpaceAlgorithm (see wiki)
 * for the sinavm. 
 * Implemnts the interface described in sina_allocator.h.
 */

#include "sina_allocator.h"
#include "sina_types.h"
#include <stdlib.h>
#include "sina_error.h"
#include "sinavm.h"
#include "sina_symbols.h"
#include <stdio.h>

static void* current_heap	= NULL;
static void* free_heap		= NULL;
static void* heap_position	= NULL;
static size_t heap_size		= 0;

/* this is not thread-safe! don't use this allocator
 * with multithreaded implementations of sina!
 */
static sinavm_data* rootset	= NULL;

#define MOVED_CHUNK 0
typedef struct {
	chunk_header header;
	chunk_header* new_chunk;
} moved_chunk;

chunk_header* move_chunk(chunk_header* old_chunk);
void collect_garbage();
void copy_chunk(chunk_header* old_chunk, 
                chunk_header* new_chunk);
void copy_list_node_chunk(list_node_chunk* old, list_node_chunk* new);
void copy_list_head_chunk(list_head_chunk* old, list_head_chunk* new);
void copy_block_chunk(block_chunk* old, block_chunk* new);

void allocate_heap(sinavm_data* vm, size_t size)
{
	rootset = vm;
	heap_size = size / 2;
	current_heap = malloc(sizeof(char) * heap_size);
	free_heap = malloc(sizeof(char) * heap_size);

	heap_position = current_heap;
}

void* allocate_chunk(int type)
{
	size_t chunk_size = sizeof_chunk(type);

	if (heap_position - current_heap + chunk_size > heap_size)
	{
		/* not enough space in current_heap */
		collect_garbage();
		if (heap_position - current_heap + chunk_size > heap_size)
		{
			/* garbage collection did not free up enough space */
			error_exit("allocate_chunk: out of memory\n");
		}	
	}

	/* we have enough space in current_heap */
	void* result = heap_position;
	heap_position += chunk_size;

	/* initialize it somewhat */
	chunk_header* ch = result;
	ch->type = type;
	ch->colour = 0; /* not used for this allocator */

	return result;
}

/* implements the TwoSpaceAlgorithm as described in the wiki */
void collect_garbage()
{
    printf("INFO: collecting_garbage\n");
	heap_position = free_heap;

	/* move all the chunks to the new current heap */
	rootset->ds = (list_head_chunk*) move_chunk((chunk_header*) rootset->ds);
	rootset->cs = (list_head_chunk*) move_chunk((chunk_header*) rootset->cs);

	/* move the symbol pointers to the new heap */
	int i;
	for (i = 0; i < SINA_SYMBOLS_MAX; ++i)
	{
		chunk_header* ch = sinavm_dereference_symbol(rootset, i);
		if (NULL != ch)
		{
			sinavm_bind(rootset, i, move_chunk(ch));
		}
	}
    
    /* move all the chunks in the registers to the new heap */
    for (i = 0; i < sina_allocator_next_free_register; ++i)
    {
        sina_allocator_register[i] 
            = move_chunk(sina_allocator_register[i]);
    }

	/* switch meaning of current heap to the free heap */
	void* temp = current_heap;
	current_heap = free_heap;
	free_heap = temp;
	/* heap_position allready points to the right heap */
}

/* allocate a now chunk with same type on the free_heap,
 * change type of old_chunk to MOVED_CHUNK and add a pointer
 * to the new chunk (do this afterwards, because we don't want
 * to overwrite the pointers...)
 * for each pointer in the other types,
 *    * if it points to a MOVED_CHUNK, copy pointer to the new chunk
 * 	  * else call move_chunk() on it first and then copy the pointer
 * return pointer to new chunk
 *
 * We can be sure that there is enough space in the free heap,
 * since it fits in the current_heap and that has exactly the
 * same size.
 */
chunk_header* move_chunk(chunk_header* old_chunk)
{
	if (NULL == old_chunk)
	{
		return NULL;
	}
	else 
	{
		if (MOVED_CHUNK == old_chunk->type)
		{
			return ((moved_chunk*) old_chunk)->new_chunk;
		}
		else
		{
			/* allocate new chunk on the free_heap */
			chunk_header* new_chunk = heap_position;
			heap_position += sizeof_chunk(old_chunk->type);
			new_chunk->type = old_chunk->type;
			new_chunk->colour = 0;
			
			/* recursion based on type */
			copy_chunk(old_chunk, new_chunk);	

			/* store pointer to new chunk in the moved chunk */
			moved_chunk* mc = (moved_chunk*) old_chunk;
			mc->header.type = MOVED_CHUNK;
			mc->new_chunk = new_chunk;

			return new_chunk;
		}
	}
}

/* copy the chunk data depending on type. if chunk contains pointers,
 * move_chunk() them unless they are allready moved
 */
void copy_chunk(chunk_header* old_chunk, 
           	    chunk_header* new_chunk)
{
	switch (old_chunk->type)
	{
		case INTEGER_CHUNK:
			((integer_chunk*) new_chunk)->value 
				= ((integer_chunk*) old_chunk)->value;
			break;

		case SYMBOL_CHUNK:
			((symbol_chunk*) new_chunk)->symbol 
				= ((symbol_chunk*) old_chunk)->symbol;
			break;

		case ESCAPED_SYMBOL_CHUNK: 
			((escaped_symbol_chunk*) new_chunk)->symbol
				= ((escaped_symbol_chunk*) old_chunk)->symbol;
			break;

		case LIST_NODE_CHUNK:
			copy_list_node_chunk(
				(list_node_chunk*) old_chunk, 
				(list_node_chunk*) new_chunk);
			break;

		case LIST_HEAD_CHUNK:
			copy_list_head_chunk(
				(list_head_chunk*) old_chunk, 
				(list_head_chunk*) new_chunk);
			break;

		case BLOCK_CHUNK:
			copy_block_chunk(
				(block_chunk*) old_chunk, 
				(block_chunk*) new_chunk);
			break;

		case NATIVE_CHUNK:
			((native_chunk*) new_chunk)->func
				= ((native_chunk*) old_chunk)->func;
			break;
		
		default:
			error_exit("copy_chunk: unknown type\n");
	}
}

void copy_list_node_chunk(list_node_chunk* old, list_node_chunk* new)
{
	new->next = (list_node_chunk*) move_chunk((chunk_header*) old->next);
	new->data = move_chunk(old->data);
}

void copy_list_head_chunk(list_head_chunk* old, list_head_chunk* new)
{
	new->first = (list_node_chunk*) move_chunk((chunk_header*) old->first);
	new->last  = (list_node_chunk*) move_chunk((chunk_header*) old->last);
}

void copy_block_chunk(block_chunk* old, block_chunk* new)
{
	new->code = (list_head_chunk*) move_chunk((chunk_header*) old->code);
	new->current = (list_node_chunk*) move_chunk((chunk_header*) old->current);
}
