/*
 * sina_builtins.c
 *
 * implements the interface defined in sina_builtins.h - see header file
 * for description of exported functions
 */
#include "sina_builtins.h"
#include "sina_error.h"
#include "sina_types.h"
#include "sina_symbols.h"
#include <stdio.h>

/* add a function to the symbol table */
void add_func(sinavm_data* vm, char* symbol, native_func f);

void add(sinavm_data* vm);
void append(sinavm_data* vm);
void swap(sinavm_data* vm);

void builtins_add(sinavm_data* vm) {
	printf("in builtins_add\n");
	add_func(vm, "add", add);
	printf("added func 'add'\n");
	add_func(vm, "swap", swap);
	printf("added func 'swap'\n");
	add_func(vm, "append", append);
	printf("added func 'append'\n");
}

void add_func(sinavm_data* vm, char* name, native_func f)
{
	int symbol = symbols_insert(name);
	printf("inserted symbol\n");
	native_chunk* n = sinavm_new_native(f);
	printf("created new native\n");
	sinavm_bind(vm, symbol, (chunk_header*) n);	
}

/* add the two top numbers in the data stack */
void add(sinavm_data* vm)
{
	if (sinavm_list_empty(vm->ds))
	{
		error_exit("add: too few arguments\n");
	}
	chunk_header* ac = vm->ds->first->data;
	sinavm_pop_front(vm->ds);
	if (INTEGER_CHUNK != ac->type)
	{
		error_exit("add: expected integer\n");
	}
	integer_chunk* ai = (integer_chunk*) ac;
	int a = ai->value;
	
	if (sinavm_list_empty(vm->ds))
	{
		error_exit("add: too few arguments\n");
	}
	chunk_header* bc = vm->ds->first->data;
	sinavm_pop_front(vm->ds);
	if (INTEGER_CHUNK != bc->type)
	{
		error_exit("add: expected integer\n");
	}
	integer_chunk* bi = (integer_chunk*) bc;
	int b = bi->value;
	
	integer_chunk* ci = sinavm_new_int(a + b);
	sinavm_push_front(vm->ds, ci);
}

/* append the chunk on top of the stack to the list
 * second from top
 */
void append(sinavm_data* vm)
{
	if (sinavm_list_empty(vm->ds))
	{
		error_exit("append: expected [list data], found empty stack\n");
	}
	else
	{
		chunk_header* data = vm->ds->first->data;
		sinavm_pop_front(vm->ds);

		if (sinavm_list_empty(vm->ds))
		{
			error_exit("append: too few arguments\n");
		}
		else
		{
			chunk_header* header = vm->ds->first->data;
			if (LIST_HEAD_CHUNK != header->type)
			{
				error_exit("append: second argument must be list\n");
			}
			else
			{
				list_head_chunk* list = (list_head_chunk*) header;
				sinavm_push_back(list, data);
			}
		}
	}
}

/* swap the too top items in the ds */
void swap(sinavm_data* vm)
{
	if (sinavm_list_empty(vm->ds))
	{
		error_exit("swap: too few arguments\n");
	}
	else
	{
		chunk_header* a = vm->ds->first->data;
		sinavm_pop_front(vm->ds);

		if (sinavm_list_empty(vm->ds))
		{
			error_exit("swap: too few arguments\n");
		}
		else
		{
			chunk_header* b = vm->ds->first->data;
			sinavm_pop_front(vm->ds);
			sinavm_push_front(vm->ds, a);
			sinavm_push_front(vm->ds, b);
		}
	}
}
