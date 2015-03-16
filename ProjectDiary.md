05.10.2007
> Implemented the TwoSpaceAlgorithm in `twospace_allocator.c` and changed the `Makefile` to allow building both versions (TwoSpaceAlgorithm and a simpler version that does no collection in `simple_allocator.c`).

> Came across an interesting problem with the TwoSpaceAlgorithm: Due to the way sinavm works, certain functions (like `sinavm_push_front()`) allocate multiple pointers to chunks on heap before returning. For instance: Allocating an `INTEGER_CHUNK` and pushing it onto the DataStack requires having the integer chunk in a local variable and then using `sinavm_push_front()` to push it onto the DataStack. This allocates a `LIST_NODE_CHUNK` to store the integer chunk in, and, if that triggers a GarbageCollectionCycle, the integer chunk will not be moved (as it is not reachable from the RootSet). This is a subtle and very hard to find error. To fix it, I implemented a RegisterStack, which is part of the RootSet. Any internal functions that hold references to chunks not reachable by the RootSet must push those chunks onto the RegisterStack before doing any operation that might trigger a GarbageCollectionCycle. This is probably not strictly necessary for the ConcurrentGarbageCollector (i.e., it will ignore the RegisterStack since it doesn't move any chunks anyway - defragmentation of memory will be handled in a much simpler way: all chunks will have the same size). Alas, the RegisterStack, as it is implemented now (`sina_allocator.c`) is not threadsafe, and does not allow the interpreter to run multiple [VirtualMachine](VirtualMachine.md)s.

25.09.2007
> Added all the builtins needed to run the CesarShift algorithm (debugged the CesarShift algorithm at the same time) and modified the Makefile to run it on the plaintext "abcdef". All there is left to do now is
  * implement the GarbageCollectors
  * devise a system for making the specific targets necessary for comparing the GCs.
  * run some comparisons
  * document the project

19.09.2007
> The interpreter now runs a small test file - will have to implement builtins for the whole of the CesarShift algorithm, but I don't expect many problems there.

> Goal for this week: Implement a GarbageCollector (The TwoSpaceAlgorithm for starters - we don't want to get our hands messy with pthreads just yet.

31.08.2007
> Fleshed out the parser to build the AST while parsing. This is done with the help of the `phelper.h` functions, but they will probably be refactored to the virtual maschine later on, since it is functionality needed there too (`sina_constructors.h`?).

> Implemented a pretty printer for sina data to verify the parser - it seems to work quite well so far.

> The symbol table is still a dummy, all symbols get mapped to id 42. This will change shortly, i intend to use the hashtable stuff in the standard library (not the GNU reentrant variant, since I don't expect to need more than one hash table for the moment).

> Moved away from the `test.sh` script for compiling and did a proper `Makefile`.

> Implemented a dummy allocator along with the relevant interface `sina_allocator.h`. This version just uses `malloc` for each allocation request - clearly, it will run out of memory soon, since there is no GC for this allocator. Garbage collectors and allocators will have to work hand in hand. I am thinking about adding an extra GC and will end up with these:
    * TwoSpaceGarbageCollector
    * DijkstraGarbageCollector
      1. with a mutex for the current "white" value
      1. use `wait` and `notify` to coordinate allocator and gc, so allocator can keep a stash of pristine chunks.

> I will flesh out these Ideas by end of next week. Also, I want to have the interpreter going by then.

24.08.2007
> Implemented a simple parser and a lexer for the SinaLanguage. So far, there aren't any actions defined, so the parser just works as an acceptor. Nevertheless, it was able to find a syntactic error in the wiki entry for the CesarShift algorithm. That page has been updated. Also, the source for the parser (sina\_parser.y), the lexer (sina\_lexer.l), a shell script for building it (test.sh) and the cesar-shift file (cesar\_shift.sina) were checked in.
> By end of next week, I plan to implement the actions so that the parser assembles the bytecode for the sina vm. The single linked list I had planned so far as the datatype for sina code and sina lists poses a problem for the parser: Apparently, I am supposed to use left recursive rules as opposed to right recursive rules. Thus, appending a new list\_element will either be O(n) for single linked list or O(1) for a more sophisticated data structure.
> I used these links as primers for flex and bison:
    * http://www.cs.utah.edu/dept/old/texinfo/bison/bison.html
    * http://ds9a.nl/lex-yacc/cvs/lex-yacc-howto.html

26.06.2007
> Project speed has been rather slow so far. Today I finally nailed the [ProjectRequirement](ProjectRequirement.md)s. This has been nagging me for a while, since it is not much of a chore (this is an exploration type project with a single main goal), but needs to be done before I can allow myself to continue with the design of the system. I expect the basic design to be finished by this time next week so I can start coding.

07.06.2007
> After watching [this video on lock-free hashtables](http://video.google.com/videoplay?docid=2139967204534450862&q=hashtable) I believe it might be possible to avoid locking in most cases for the GC. In Dijkstras GC algorithm, only the GC ever changes the current colour. The allocator (running in the interpreter thread) needs only to read the current colour and notice when it changes (compare it to a stored copy of the current value).

> The GC could therefore write the new colour and wait for the interpreter to pick up the change. I could use wait() and notify() here, since the GC doesn't really have to do anything until the interpreter wants to allocate more memory. But this would be designing for at most one worker thread, something I would prefer to avoid.

> What about allocation? Suppose we keep a linked list of all free memory chunks, preferably in ready-to-use sizes. Some mechanism would be needed to stop the GC and the allocator from interfering, since both of them are writing to the same list. If we rule that the GC only adds recycled chunks at the end of the list and the allocator only removes chunks from the front of the list, nobody is really in each others way: It wouldn't matter in which order the GC and the allocator arrive at the array, since the GC will traverse the list all the way to the end. If it finds it's not in the free list anymore, it just retries. Following the free list will have to include checking the next pointer for sanity (inside the heap? Don't want to risk a segmentation fault) and while traversing make sure the nodes are all free and of the correct node type. This will have to be thought through, just to be sure we don't run into a race condition here.

> An alternate idea would be to have two free lists: One maintained by the collector, the other maintained by the allocator. When the allocator runs out of space, it notifies the collector (write a flag, same problem as above with the current colour) and waits for the collector to feed it more memory.

> This all doesn't solve memory defragmentation. Sadly, that is an issue not addressed by Dijkstras GC algorithm. I'm not quite sure if I should come up with a solution or just leave it like it is. With the SinaLanguage design, chunks only ever come in fixed sizes, so compacting might not be an issue at all anyway.


