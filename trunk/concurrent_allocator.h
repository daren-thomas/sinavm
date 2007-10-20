#ifndef CONCURRENT_ALLOCATOR_H
#define CONCURRENT_ALLOCATOR_H

/* concurrent_allocator.h
 *
 * Declares functions used in concurrent_allocator.c
 */
#include <stdlib.h>
#include "sina_types.h"

#define FREE_CHUNK 0
typedef list_node_chunk free_chunk;

/* string n free_chunks starting at heap h together to form
 * a list and return a pointer to first chunk.
 */
free_chunk* allocate_chunk_list(free_chunk* h, size_t n);

/* Synchronizes with collector thread to recieve a new free_list.
 * If resulting free_list is empty, try once more, then abort with
 * an error message.
 *
 * Post condition: free_list points to a list of chunks that can
 * be used or program is aborted.
 */
void mutator_await_free_list();

/* whenever a push_front/back is called by the interpreter, we know
 * that these chunks are accessible - darken them, since collector
 * might miss them in a race condition.
 */
list_head_chunk* mutator_push_hook(list_head_chunk* list, chunk_header* data);

/* implements the collector thread, as explained in the wiki */
void* collector_main(void*);

/* collects garbage (darkens nodes until no more grey nodes
 * are found, builds the collector_list from the white nodes
 * marking them green)
 */
void collector_collect_garbage();

/* marks the data stack, the code stack and any defined symbols
 * grey.
 */
void collector_mark_root_chunks_grey();

/* finds the next grey chunk starting at 'chunk' and returns a pointer
 * to it. If the end of the heap is reached and no grey chunk could be
 * found, search is wrapped to beginning of heap (until all chunks in
 * the heap have thus been checked and found to be not grey).
 * If no grey chunks could be found, returns NULL.
 */
chunk_header* collector_find_grey_chunk(chunk_header* chunk);

/* darkens the successors to chunk in the following manner: if they
 * are found to be white, colour them grey. Otherwise, leave them as
 * is.
 * for data types (INTEGER_CHUNK, SYMBOL_CHUNK, ESCAPED_SYMBOL_CHUNK
 * and NATIVE_CHUNK), returns without doing anything.
 */
void collector_darken_successors(chunk_header* chunk);

/* darkens the chunk in the following manner: if it is found
 * to be white, colour it grey. Otherwise, leave it as is. 
 */
void collector_darken_chunk(chunk_header* chunk);

/* iterates over the heap and strings together a list of all white
 * nodes, changing them into FREE_CHUNKs, colouring them green at the
 * same time.
 * the collector_list points to the first free_chunk thus found or to
 * NULL if no white chunks were found.
 */
void collector_build_collector_list();

#endif
