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

void allocate_heap(size_t size);

void* allocate_chunk(int type);

#endif
