# Introduction #

The ConcurrentGarbageCollector used in the sinavm is inspired by the [algorithm proposed by Dijkstra for On-the-fly garbage collection](http://www.cs.utexas.edu/users/EWD/transcriptions/EWD04xx/EWD492.html). Since the way `sina` works is not entirely different from lisp, the algorithm can be implemented. But, sollutions have to be found for the details overlooked in Dijkstras description:

1. How is the switch between white/black interpretation synchronised?
2. How can the free list be appended to by the collector and consumed by the
> mutator at the same time?

Also: the sinavm has more than one root (DataStack, CodeStack and the symbols), these will be traversed sequentially, applying the same algorithm to them all.

# Details #

I propose following sollution to the two "details" mentioned above: Mutator and collector each keep a separate list, the `free_list` and the `collector_list` respectively. Synchronisation happens when the `free_list` is empty. The mutator then signals the collector that it needs a new `free_list` and waits for the collector to move the `collector_list` to the `free_list` (only a pointer needs to be copied). Then, the collector swaps the values for black (`black_value`) and white (`white_value`) and signals the mutator that the `free_list` is now populated.

For this to work, following situations have to be accounted for:

1. Mutator empties `free_list` before collector has built the `collector_list`
2. Collector has built the `collector_list` before the mutator empties the `free_list`

I propose following synchronisation protocol:

A guarded flag `flag_freelist_empty` (guarded by `mutex_freelist_empty`) that gets set by the mutator, signals that the mutator needs a new freelist. The collector, upon finishing its work, locks `mutex_freelist_empty` and checks `flag_freelist_empty`. If it is not set (`0`), then it waits on `cond_freelist_empty`, thus unlocking `mutex_free_list_empty`. The mutator will allways signal `cond_freelist_empty` to unlock the mutex -> no matter what, after these steps, the collector has the `mutex_freelist_empty` and `flag_freelist_empty` is set to `1`. The collector is responsible for cleaning up the flag (setting it to `0`).

A guarded flag `flag_freelist_ready` (guarded by `mutex_freelist_ready`) gets set by the collector, which also signals `cond_freelist_ready` in the manner above (thus no matter when mutator checks `flag_freelist_ready`, at the end of this synchronisation, it has the mutex and the flag is set). At this point, the collector has copied the `collector_list` pointer to the `free_list` pointer. The mutator is responsible for cleaning up the flag (setting it to `0`).

At the end of synchronisation, the free list contains all the nodes harvested by the collector and both flags are set to 0.

# Implementing the mutator #

The algorithm depends on the mutator colouring nodes whenever they are made accessible from the RootSet. Since the sinavm also has an implementation of the TwoSpaceAlgorithm, which does not have these requirements, a hook into the interpreter needs to be made, that can be used by allocators if needed. These hooks are needed for `sinavm_push_front`, `sinavm_push_back` and `sinavm_pop_front`. The mutator will use these hooks to do the colouring, if the list being pushed / popped happens to be a RootNode.

# Initialization #

## freelist ##

At the start, there are no nodes in the RootSet. Thus, all nodes added to the RootSet end up being black. The collector can not find any white chunks and the `collector_list` will remain empty. In this case, the allocator will get an empty `free_list` after allocation and have to wait for the collector to rewalk the heap and try again - if this fails, then we are sure we don't have any memory left.

This is acceptable during execution of the program as it is not possible to know in advance when and if this will occurr.

**But**: The first time the mutator runs out of chunks on the free list, we know for sure, that the collector cannot have found any free chunks. So, we initialize the `free_list` to only half of the available heap and keep the rest in `collector_list`. Then, we can also have the collector start waiting on `cond_freelist_empty`.

## Heap ##

The heap is regarded as an iterable array. Thus, each chunk needs to be the same size (the size can't be determined by checking it's type, since this could result in race conditions and also: what is the size of a free (green) chunk?). This wastes some space (INTEGER\_CHUNKS, SYMBOL\_CHUNKS etc. are 1 word smaller than LIST\_HEAD\_CHUNKS, LIST\_NODE\_CHUNKS and BLOCK\_CHUNKS), but it is necessary for the collector to be able to iterate over all chunks.