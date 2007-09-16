/*
 * sinavm.c
 *
 * implements the functions defined in sinavm.h (see header file
 * for comments on the functions)
 */

#include "sina_allocator.h"
#include "sina_types.h"
#include "sinavm.h"
#include <string.h>
#include "sina_builtins.h"
#include <stdlib.h>
#include <stdio.h>

void sinavm_initialize(sinavm_data* vm)
{
	printf("in sinavm_initialize\n");
	vm->cs = sinavm_new_list();
	printf("initialized cs\n");
    vm->ds = sinavm_new_list();
	printf("initialized ds\n");
	vm->bindings = malloc(sizeof(chunk_header*) * SINAVM_MAX_SYMBOLS);
	printf("initialized bindings\n");
    
	int i;
    for (i = 0; i < SINAVM_MAX_SYMBOLS; ++i)
    {
        (vm->bindings)[i] = NULL;
    }
   	printf("before builtins_add\n"); 
    builtins_add(vm);
}

list_head_chunk* sinavm_new_list()
{
	list_head_chunk* result = (list_head_chunk*) 
		allocate_chunk(LIST_HEAD_CHUNK);

	result->first = NULL;
	result->last  = NULL;
	return result;
}

list_head_chunk* sinavm_push_back(list_head_chunk* list, void* data)
{
	list_node_chunk* node = (list_node_chunk*)
		allocate_chunk(LIST_NODE_CHUNK);

	node->data = data;
	node->next = NULL;

	if (list->first == NULL) 
	{
		list->first = node;
		list->last = node;
	}
	else
	{
		list_node_chunk* last = list->last;
		last->next = node;
		list->last = node;
	}
	return list;
}

list_head_chunk* sinavm_push_front(list_head_chunk* list, void* data)
{
    list_node_chunk* node = (list_node_chunk*)
        allocate_chunk(LIST_NODE_CHUNK);
    
    node->data = data;
    node->next = list->first;
    list->first = node;
    
    if (list->last == NULL)
    {
        /* list was empty on call */
        list->last = node;
    }
    return list;
}

list_head_chunk* sinavm_pop_front(list_head_chunk* list)
{
    if (list->first != NULL)
    {
        if (list->first == list->last)
        {
            /* only one element left in list */
            list->first = NULL;
            list->last  = NULL;
        }
        else
        {
            list->first = list->first->next;
        }
    }
}

integer_chunk* sinavm_new_int(int value)
{
	integer_chunk* result = (integer_chunk*)
		allocate_chunk(INTEGER_CHUNK);

	result->value = value;
	return result;
}

symbol_chunk* sinavm_new_symbol(int symbol)
{
	symbol_chunk* result = (symbol_chunk*)
		allocate_chunk(SYMBOL_CHUNK);

	result->symbol = symbol;
	return result;
}

escaped_symbol_chunk* sinavm_new_escaped_symbol(int symbol)
{
	escaped_symbol_chunk* result = (escaped_symbol_chunk*)
		allocate_chunk(ESCAPED_SYMBOL_CHUNK);

	result->symbol = symbol;
	return result;
}

block_chunk* sinavm_new_block(list_head_chunk* list)
{
	block_chunk* result = (block_chunk*)
		allocate_chunk(BLOCK_CHUNK);

	result->code = list;
	result->current = list->first;
	return result;
}

native_chunk* sinavm_new_native(native_func f)
{
	native_chunk* result = (native_chunk*)
		allocate_chunk(NATIVE_CHUNK);

	result->func = f;
	return result;
}

list_head_chunk* sinavm_new_string(char* string)
{
	list_head_chunk* result = sinavm_new_list();
	for (; *string; ++string)
	{
		int c = *string;
		integer_chunk* i = sinavm_new_int(c);
		result = sinavm_push_back(result, i);
	}
	return result;
}

int sinavm_list_empty(list_head_chunk* list)
{
    if (list->first == NULL && list->last == NULL)
    {
        /* list is empty */
        return 1;
    }
    else
    {
        /* list has at least 1 element */
        return 0;
    }
}

chunk_header* sinavm_dereference_symbol(sinavm_data* vm, int symbol)
{
    return (vm->bindings)[symbol];
}

void sinavm_bind(sinavm_data* vm, int symbol, chunk_header* data)
{
	if (symbol >= SINAVM_MAX_SYMBOLS)
	{
		error_exit("symbol range exceeded.");
	}
	else
	{
		(vm->bindings)[symbol] = data;
	}
}
