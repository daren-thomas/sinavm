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
 
/* type used to denote free_chunks in the free_list and the collector_list 
 * is actually a LIST_NODE_CHUNK, with data set to NULL and type FREE_CHUNK.
 */
#define FREE_CHUNK 0

/* the free_list is read and written by allocator until it points to NULL,
 * then synchronisation with collector thread takes place, and while collector
 * owns mutex_freelist_ready, it can be changed by collector.
 * the collector_list is only ever touched by the collector thread
 */
list_node_chunk* free_list      = NULL;
list_node_chunk* collector_list = NULL;

4