#include "sina_interpreter.h"
#include "sina_types.h"
#include "sinavm.h"
#include "sina_error.h"
#include <stdio.h>

void interpret_chunk(sinavm_data* vm, chunk_header* node);
void interpret_symbol(sinavm_data* vm, int symbol);
void execute_block(sinavm_data* vm, block_chunk* block);

/*
 * sina_interpreter.c
 *
 * implements the sina interpreter. See the header file
 * sina_interpreter.h for overview on how the interpreter
 * works.
 */
void sina_interpret(sinavm_data* vm, block_chunk* code)
{
    sinavm_push_front(vm->cs, code);
    
    while ( ! sinavm_list_empty(vm->cs))
    {
		printf("--------------------------------------------------------------------------------------\n");
		pprint_vm_state(vm);
        block_chunk* current_block = (block_chunk*) vm->cs->first->data;
        if (NULL == current_block->current)
        {
            /* block is now interpreted, pop it off stack */
            sinavm_pop_front(vm->cs);
        }
        else
        {
            list_node_chunk* current = current_block->current;                       
            interpret_chunk(vm, (chunk_header*)current->data);
            
            current_block->current = current_block->current->next;
        }
    }
}

void interpret_chunk(sinavm_data* vm, chunk_header* header)
{
    escaped_symbol_chunk*  esym     = NULL;
    symbol_chunk*          symbol   = NULL;
	native_chunk*		   native	= NULL;
    
    switch (header->type)
    {
        case INTEGER_CHUNK:
        case LIST_HEAD_CHUNK:
            /* push chunk onto DS */
			sinavm_push_front(vm->ds, header);
			break;       
        
        case BLOCK_CHUNK:
            /* push block onto ds, to execute a block_chunk: use 'call' or bind symbol */
			sinavm_push_front(vm->ds, header);
			break;
        
        case ESCAPED_SYMBOL_CHUNK:
			/* push symbol onto DS */
            esym = (escaped_symbol_chunk*) header;
            sinavm_push_front(vm->ds, sinavm_new_symbol(esym->symbol));
			break;

		case SYMBOL_CHUNK:
			symbol = (symbol_chunk*) header;
            interpret_symbol(vm, symbol->symbol);
			break;

		case NATIVE_CHUNK:
			native = (native_chunk*) header;
			native->func(vm);
			break;
        
        default:
            error_exit("cannot interpret chunk with unknown type.");
    }
}

/* symbols need to be interpreted specially (see sina_interpreter.h) */
void interpret_symbol(sinavm_data* vm, int symbol)
{
    chunk_header* header = sinavm_dereference_symbol(vm, symbol);
    if (NULL == header)
    {
        error_exit("cannot interpret unbound symbol");
    }
    else
    {
        if (BLOCK_CHUNK == header->type)
        {
            sinavm_execute_block(vm, (block_chunk*) header);
        }
        else
        {
            interpret_chunk(vm, header);
        }
    }
}
