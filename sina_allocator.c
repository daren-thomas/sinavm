#include "sina_types.h"
#include "sina_allocator.h"
#include "sina_error.h"

chunk_header* sina_allocator_register[SINA_ALLOCATOR_MAX_REGISTERS];
int sina_allocator_next_free_register = 0;

/* add chunk to list of chunks that might get reallocated */
void allocate_push_register(chunk_header* chunk)
{
    error_assert(sina_allocator_next_free_register < SINA_ALLOCATOR_MAX_REGISTERS,
        "allocate_push_register: registers full\n");
    
    sina_allocator_register[sina_allocator_next_free_register++] = chunk;
}

/* pop chunks from list of chunks that might have been reallocated,
 * sequence is maintained
 */
chunk_header* allocate_pop_register()
{
    error_assert(sina_allocator_next_free_register > 0,
        "allocate_pop_register: cannot pop empty stack\n");
    
    return sina_allocator_register[--sina_allocator_next_free_register];
}