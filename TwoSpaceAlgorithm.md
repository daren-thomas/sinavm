# Introduction #

This page describes the TwoSpaceAlgorithm, as it is implemented for the sinavm. As such, the algorithm might not be described as neutrally as possible, but with it's application to the task at hand in mind.


# Details #

GarbageCollectors in the sinavm implement the interface defined in `sina_allocator.h`. The TwoSpaceAlgorithm only works on half of the allowed heap size (as set by the call to `void allocate_heap(size_t size)` at a time - when it is used up, all reachable objects are copied to the second heap partition and the roles switched.

So, we have two heap partitions:

  * `current_heap`
  * `free_heap`

Each of these heaps has (the same) size, half the size of the reserved space (`size`) for the vm.

  * `heap_size`

We also know the position of the next free block of memory (initially == `current_heap`)

  * `heap_position`

Garbage collection only takes place, when the size of an object that has to be allocated is too big to fit in the remaining space in the current heap. This can be calculated by adding the size of the new object to the `heap_position` and comparing that to `current_heap + heap_size`.

Garbage collection consists of moving each chunk to the `free_heap` and then swapping the `free_heap` and `current_heap` pointers. During garbage collection the `heap_position` points to the next free position in the `free_heap`.

To copy a chunk to the free heap (`move_chunk()`),

  * allocate a new chunk with same type on the `free_heap`
  * change type of old chunk to `MOVED_CHUNK` and add a pointer to the new chunk
  * for integers, symbols and escaped symbols, copy the value to the new chunk
  * for each pointer in the other types,
    * if it points to a `MOVED_CHUNK`, copy pointer to the new chunk
    * else apply copy algorithm (`move_chunk()`) to pointed-to-chunk first and then copy the pointer to the new chunk (recursion)
  * return pointer to new chunk

`move_chunk()` is applied to the DS, the CS and to each bound symbol. Any chunk not reached in this manner is considered garbage.