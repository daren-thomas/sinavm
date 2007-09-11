/*
 * phelper.c
 *
 * implements the functions defined in phelper.h (see header file
 * for comments on the functions)
 */

#include "sina_allocator.h"
#include "sina_types.h"

list_head_chunk* phelper_new_list()
{
	list_head_chunk* result = (list_head_chunk*) 
		allocate_chunk(LIST_HEAD_CHUNK);

	result->first = NULL;
	result->last  = NULL;
	return result;
}

list_head_chunk* phelper_push_back(list_head_chunk* list, void* data)
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

integer_chunk* phelper_new_int(int value)
{
	integer_chunk* result = (integer_chunk*)
		allocate_chunk(INTEGER_CHUNK);

	result->value = value;
	return result;
}

symbol_chunk* phelper_new_symbol(int symbol)
{
	symbol_chunk* result = (symbol_chunk*)
		allocate_chunk(SYMBOL_CHUNK);

	result->symbol = symbol;
	return result;
}

escaped_symbol_chunk* phelper_new_escaped_symbol(int symbol)
{
	escaped_symbol_chunk* result = (escaped_symbol_chunk*)
		allocate_chunk(ESCAPED_SYMBOL_CHUNK);

	result->symbol = symbol;
	return result;
}

block_chunk* phelper_new_block(list_head_chunk* list)
{
	block_chunk* result = (block_chunk*)
		allocate_chunk(BLOCK_CHUNK);

	result->code = list;
	result->current = list->first;
	return result;
}

list_head_chunk* phelper_new_string(char* string)
{
	list_head_chunk* result = phelper_new_list();
	for (; *string; ++string)
	{
		int c = *string;
		integer_chunk* i = phelper_new_int(c);
		result = phelper_push_back(result, i);
	}
	return result;
}
