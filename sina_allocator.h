#ifndef SINA_ALLOCATOR_H
#define SINA_ALLCATOR_H

/*
 * sina_allocator.h
 *
 * defines the interface for allocators as used by the sinavm and
 * the sina parser.  
 * The allocated piece of memory is expected to be of the right
 * size for the requested type. The chunk header must be filled
 * with the correct type and "colour" (a value indicating that
 * the chunk is in use).
 * If the allocator runs out of memory for allocation then it
 * must stop the execution of the application immediatly.
 * The function allocate_heap is called once before the first
 * call of allocate_chunk and can be used to reserve some memory.
 */

#include <stddef.h>
#include "sinavm.h"
#include "sina_types.h"

void allocate_heap(sinavm_data* vm, size_t size);

/* during allocation,chunks might be reallocated. Reallocation will
 * move any chunks reachable through the vm aka rootset (bound symbols, 
 * ds, cs).
 * If any chunks are allocated but not reachable through the rootset,
 * then they must be added to the regs structure so they can be moved
 * too.
 */
void* allocate_chunk(int type);

/* ------------------------------------------------------------------------------ 
 * The following functions need not be implemented in the allocator
 * implementations: they are implemented in sina_allocator.c
 * ------------------------------------------------------------------------------ 
 */
#define SINA_ALLOCATOR_MAX_REGISTERS 16
extern chunk_header* sina_allocator_register[SINA_ALLOCATOR_MAX_REGISTERS];
extern int sina_allocator_next_free_register; /* next free register */

/* add chunk to list of chunks that might get reallocated */
void allocate_push_register(chunk_header* chunk);

/* pop chunks from list of chunks that might have been reallocated,
 * sequence is maintained
 */
chunk_header* allocate_pop_register();

#endif
