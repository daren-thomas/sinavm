/* concurrent_allocator.c
 *
 * Implements a concurrent garbage collector / allocator pair for the sinavm.
 * The algorithm is described in the wiki (ConcurrentGarbageCollector).
 *
 * Because of the way the Makefile is written, it would be hard to split this
 * up into two files, so, any function meant to be called by the mutator
 * is prefixed with "mutator_" and any function meant to be called by the
 * collector is prefixed with "collector_". 
 *
 * The "allocate_*" functions are called by the sinavm and are run in the 
 * mutator (=main) thread.
 */
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "sina_allocator.h"
#include "sina_types.h"
#include "sina_symbols.h"
#include "sinavm.h"
#include "concurrent_allocator.h"

void collector_print_heap();
 
/* the free_list is read and written by allocator until it points to NULL,
 * then synchronisation with collector thread takes place, and while collector
 * owns mutex_freelist_ready, it can be changed by collector.
 * the collector_list is only ever touched by the collector thread
 */
free_chunk* free_list      = NULL;
free_chunk* collector_list = NULL;

size_t      count_chunks   = 0; /* size of heap (amount of chunks allocatable)*/
free_chunk* heap           = NULL;

/* we need a copy of the vm to know the rootset */
sinavm_data* vm = NULL;

int             flag_freelist_empty  = 1; /* true, whenever free_list == NULL */
pthread_mutex_t mutex_freelist_empty = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_freelist_empty  = PTHREAD_COND_INITIALIZER;

int flag_freelist_ready    = 0; /* true, when collector has copied freelist */
pthread_mutex_t mutex_freelist_ready   = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_freelist_ready    = PTHREAD_COND_INITIALIZER;

/* the different colour values. black and white can be swaped by
 * the collector, but only when in control of
 * mutex_freelist_ready
 */
volatile int black_value   = 0; /* volatile suppresses optimization (const) */
volatile int white_value   = 2;
int grey_value    = 1;
int green_value   = 3;

/* allocating a heap means creating the free_list, the collector_list
 * and starting the collector thread. size is given as an amount of
 * bytes, but must be converted into a multiple of sizeof(free_chunk), 
 * which is supposed to be the largest chunk size available.
 */
void allocate_heap(sinavm_data* _vm, size_t size)
{
	vm = _vm;

	count_chunks = size / sizeof(free_chunk);
	heap = malloc(count_chunks * sizeof(free_chunk));
	free_list = allocate_chunk_list(heap, count_chunks / 2);
	flag_freelist_empty = 0;
	collector_list = allocate_chunk_list(heap + count_chunks / 2, 
		count_chunks / 2);

	if (sinavm_trace_get(vm))
	{	
		printf("allocate_heap: allocated chunk lists\n");
	}

	/* set up hooks for monitoring changes made by monitor
	 * (in essence, anytime a list gets pushed / popped, darken the
	 * list and the node to be appended to. Also darken the data being pushed
	 */
	sinavm_push_front_hook = mutator_push_hook;
	sinavm_push_back_hook  = mutator_push_hook;
	sinavm_pop_front_to_register_hook = mutator_pop_register_hook;

	/* start the collector thread */
	pthread_t collector_thread;
	int rc = pthread_create(&collector_thread, NULL, collector_main, NULL);
	error_assert(rc == 0, "allocate_heap: failed to create collector thread\n");
	collector_print_heap();
}

/* allocating is easy, if the free_list still has chunks left: pop
 * one of the front, add colour and type and we are done! 
 * otherwise, we have to signal the collector, that we want some new
 * chunks.
 */
void* allocate_chunk(int type)
{
	if (NULL == free_list)
	{
		if (sinavm_trace_get(vm))
		{
			printf("allocate_chunk: free_list empty\n");
		}

		mutator_await_free_list(); /* crashes if not successfull */	
	}
	error_assert(NULL != free_list, "allocate_chunk: free_list empty\n");

	/* get next chunk of free_list */
	free_chunk* result = free_list;
	free_list = free_list->next;	

	result->next = NULL; /* clean up memory, this can be omitted */
	result->data = NULL;
	result->header.colour = black_value;
	result->header.type = type;
	return result;
}

/* string n chunks from heap h together and return pointer
 * to first chunk in the list (=h).
 * only meaningfull if n > 0;
 */
free_chunk* allocate_chunk_list(free_chunk* h, size_t n)
{
	free_chunk* current = NULL;
	free_chunk* end     = NULL; /* one past end of list */
	for (current = h, end = h + n; current < end; ++current)
	{
		current->header.colour = green_value;
		current->header.type = FREE_CHUNK;
		current->next = current + 1;
		current->data = NULL;
	}
	free_chunk* last = end - 1;	
	last->next = NULL;
	return h;
}

/* mutator has run out of chunks (free_list empty).
 * synchronise with collector to get a new free_list.
 */
void mutator_await_free_list()
{
	int tries = 0; 
	while (tries++ < 2)
	{
		if (sinavm_trace_get(vm))
		{
			printf("mutator_await_free_list: tries = %d\n", tries);
		}
		
		pthread_mutex_lock(&mutex_freelist_empty);
		error_assert(0 == flag_freelist_empty,
			"mutator_await_free_list: expected flag_freelist_empty = 0\n");
		flag_freelist_empty = 1;
		/* collector might be waiting for this */
		pthread_cond_signal(&cond_freelist_empty); 
		/* collector mustn't signal before wait */
		pthread_mutex_lock(&mutex_freelist_ready); 
		/* let collector wake up / read flag */
		pthread_mutex_unlock(&mutex_freelist_empty);
		pthread_cond_wait(&cond_freelist_ready, &mutex_freelist_ready);
		/* we only need the signal, discard the mutex */
		pthread_mutex_unlock(&mutex_freelist_ready);

		/* now, a new freelist has been set up. If it is ok,
		 * we can return, else, we retry and die a horrible death if no
		 * free chunks could be collected
		 */
		if (NULL != free_list)
		{
			break;
		}
	}
	if (NULL == free_list)
	{
		error_exit("mutator_await_free_list: no free chunks found\n");
	}
	else
	{
		/* all is well, free chunks were found and added to free_list */
		if (sinavm_trace_get(vm))
		{
			printf("mutator_await_free_list: free_list recieved\n");
		}
		return;
	}
}

/* we now know that list and data (and the first node in list) can be reached
 * from the rootset. Darken them towards grey
 */
list_head_chunk* mutator_push_hook(list_head_chunk* list, chunk_header* data)
{
	collector_darken_chunk((chunk_header*) list);
	collector_darken_successors((chunk_header*) list);
	collector_darken_chunk( data);
    return list;
}

list_head_chunk* mutator_pop_register_hook(list_head_chunk* list)
{
	if (!sinavm_list_empty(list))
	{
		chunk_header* chunk = list->first->data;
		chunk->colour = grey_value;
	}
	return list;
}

/* wait for signal to fill free_list with the nodes in collector_list, then
 * start collecting collector_list, repeat.
 * 
 * The arguments and return value are just signature needed by the pthreads
 * library.
 */
void* collector_main(void* args)
{
	while (1) /* loop forever */
	{
		if (sinavm_trace_get(vm))
		{
			printf("collector_main: waiting for freelist_empty\n");
		}
		pthread_mutex_lock(&mutex_freelist_empty);
		if(1 != flag_freelist_empty)
		{
			pthread_cond_wait(&cond_freelist_empty, &mutex_freelist_empty);
			/* mutex_freelist_empty was released and reaquired */
			error_assert(1 == flag_freelist_empty, 
				"collector_main: expeced flag_freelist_empty = 1\n");
		}

		if (sinavm_trace_get(vm))
		{
			printf("collector_main: free_list is empty\n");
		}

		collector_print_heap();
		flag_freelist_empty = 0; /* we now know the list was empty */
		pthread_mutex_unlock(&mutex_freelist_empty);

		pthread_mutex_lock(&mutex_freelist_ready);
		/* mutator is allready waiting, because of how it overlapped
		 * the locking of the mutexes
		 */

		/* assign free_list */
		free_list      = collector_list;
		collector_list = NULL;

		if (sinavm_trace_get(vm))
		{
			printf("collector_main: assigned free_list\n");
		}

		/* swap colours */
		int temp = black_value;
		black_value = white_value;
		white_value = temp;

		if (sinavm_trace_get(vm))
		{
			printf("collector_main: swapped colours, black is now %d\n",
				black_value);
		}

		/* BUG FIX: any chunks in the registers might not be accessible
		   and have just been coloured white (by swapping colours).
		   Colour them grey to be sure the are not collected.
		   We know the registry is not being altered at the moment,
		   since the mutator is waiting for the freelist. 
		 */
		collector_colour_registers_grey();

		pthread_cond_signal(&cond_freelist_ready);
		pthread_mutex_unlock(&mutex_freelist_ready);

		/* walk the heap, talk the heap */
		collector_collect_garbage();
	}	
	return NULL;
}

/* collectos garbage (darkens nodes until no more
 * grey nodes are found, builds the collector_list from the
 * white nodes marking them green.
 */
void collector_collect_garbage()
{
	if (sinavm_trace_get(vm))
	{
		printf("collector_collect_garbage: collecting garbage\n");
	}

	collector_mark_root_chunks_grey();

	chunk_header* chunk = (chunk_header*) heap;
	while (NULL != (chunk = collector_find_grey_chunk(chunk)))
	{
		collector_darken_successors(chunk);
		chunk->colour = black_value;
	}
	collector_build_collector_list();
}

/* marks the data stack, the code stack and any defined symbols
 * grey.
 * FIXME: possible race condition with symbols (need hook into
 * binding of symbols - colour them grey...)
 */
void collector_mark_root_chunks_grey()
{
	vm->ds->header.colour = grey_value;
	vm->cs->header.colour = grey_value;

	int i;
	for (i = 0; i < SINA_SYMBOLS_MAX; ++i)
	{
		chunk_header* ch = sinavm_dereference_symbol(vm, i);
		if (NULL != ch)
		{
			ch->colour = grey_value;
		}
	}
}

/*  finds the next grey chunk starting at 'chunk' and returns a pointer
 * to it. If the end of the heap is reached and no grey chunk could be
 * found, search is wrapped to beginning of heap (until all chunks in
 * the heap have thus been checked and found to be not grey).
 * If no grey chunks could be found, returns NULL.
 */
chunk_header* collector_find_grey_chunk(chunk_header* chunk)
{ 
	free_chunk* c = (free_chunk*) chunk; /* easier to compare with heap */
	free_chunk* end_of_heap = heap + count_chunks; /* one past last chunk */

	error_assert(c >= heap && c < end_of_heap, 
		"collector_find_grey_chunk: invalid chunk pointer\n");
	
	if (grey_value == c->header.colour)
	{
		return (chunk_header*) c;
	}
	else
	{
		while (++c != (free_chunk*) chunk)
		{
			if (c >= end_of_heap)
			{
				c = heap;
			}

			if (grey_value == c->header.colour)
			{
				return (chunk_header*) c;
			}
		}
		/* cycled through all chunks in heap, c == chunk */
		if (sinavm_trace_get(vm))
		{
			printf("collector_find_grey_chunk: could not find any grey chunks "
			   "in heap...\n");
		}
		return NULL;
	}
}

/* darkens the successors to chunk in the following manner: if they
 * are found to be white, colour them grey. Otherwise, leave them as
 * is.
 * for data types (INTEGER_CHUNK, SYMBOL_CHUNK, ESCAPED_SYMBOL_CHUNK
 * and NATIVE_CHUNK), returns without doing anything.
 */
void collector_darken_successors(chunk_header* chunk)
{
	/* cast chunk to apropriate type to find successors */
	list_head_chunk* list  = NULL;
	list_node_chunk* node  = NULL;
	block_chunk*     block = NULL;

	switch (chunk->type)
	{
		case INTEGER_CHUNK:
		case SYMBOL_CHUNK:
		case ESCAPED_SYMBOL_CHUNK:
		case NATIVE_CHUNK:
		case FREE_CHUNK:
			/* do nothing */
			break;

		case LIST_HEAD_CHUNK:
			list = (list_head_chunk*) chunk;
			collector_darken_chunk((chunk_header*) list->first);
			collector_darken_chunk((chunk_header*) list->last);
			break;

		case LIST_NODE_CHUNK:
			node = (list_node_chunk*) chunk;
			collector_darken_chunk((chunk_header*) node->next);
			collector_darken_chunk((chunk_header*) node->data);
			break;

		case BLOCK_CHUNK:
			block = (block_chunk*) chunk;
			collector_darken_chunk((chunk_header*) block->code);
			collector_darken_chunk((chunk_header*) block->current);
			break;

		default:
			pprint_chunk_info(chunk);
			error_exit("collector_darken_successors: unknown chunk type\n");
	}
}

/* darkens the chunk in the following manner: if it is found to be white, 
 * colour it grey. Otherwise, leave it as is.
 */
void collector_darken_chunk(chunk_header* chunk)
{
	if (NULL != chunk)
	{
		if (white_value == chunk->colour)
		{
			chunk->colour = grey_value;	
		}
		else
		{
			/* leave colour as is (could be grey or black, but can't be sure) */
		}
	}
	else
	{
		/* chunk was a NULL pointer, whistle and move on */
	}
}

/* iterates over the heap and strings together a list of all white
 * nodes, changing them into FREE_CHUNKs, colouring them green at the
 * same time.
 * the collector_list points to the first free_chunk thus found or to
 * NULL if no white chunks were found.
 */
void collector_build_collector_list()
{
	if (sinavm_trace_get(vm))
	{
		printf("collector_build_collector_list: begin\n");
	}
	
	free_chunk* chunk = NULL;
	free_chunk* end_of_heap = heap + count_chunks;

	collector_list = NULL;

	for (chunk = heap; chunk < end_of_heap; ++chunk)
	{
		if (white_value == chunk->header.colour)
		{
			chunk->header.colour = green_value;
			chunk->header.type   = FREE_CHUNK;
			chunk->data          = NULL;
			chunk->next          = collector_list;

			collector_list = chunk;
		}
	}

	if (sinavm_trace_get(vm))
	{
		printf("collector_build_collector_list: end\n");
	}
}

void collector_colour_registers_grey()
{
	int i = 0;
	for (i = 0; i < sina_allocator_next_free_register; ++i)
	{
		chunk_header* chunk = sina_allocator_register[i];
		chunk->colour = grey_value;
	}
}

void collector_print_heap()
{
/*
	printf("\ncollector_print_heap: black=%d, white=%d\n",
		black_value, white_value);
	free_chunk* chunk = heap;
	free_chunk* end_of_heap = heap + count_chunks;
	for (; chunk < end_of_heap; ++chunk)
	{
		volatile free_chunk* c = chunk;
		volatile int type = c->header.type;
		volatile int colour = c->header.colour;
		printf("%x\t%d\t%d\t%x\t%x\n", chunk - heap,
			colour, type, 
			c->next, c->data);
	}
*/
}
