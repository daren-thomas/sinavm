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
#include "sina_interpreter.h"
#include "sinavm.h"
#include <stdio.h>

/* add a function to the symbol table */
void add_func(sinavm_data* vm, char* symbol, native_func f);

void add(sinavm_data* vm);
void append(sinavm_data* vm);
void swap(sinavm_data* vm);
void bind(sinavm_data* vm);
void call(sinavm_data* vm);
void loop(sinavm_data* vm);
void _if(sinavm_data* vm);
void equals(sinavm_data* vm);

void builtins_add(sinavm_data* vm) {
	add_func(vm, "add", add);
	add_func(vm, "swap", swap);
	add_func(vm, "append", append);
	add_func(vm, "bind", bind);
    add_func(vm, "call", call);
    add_func(vm, "loop", loop);
    add_func(vm, "if", _if);
    add_func(vm, "equals", equals);
}

void add_func(sinavm_data* vm, char* name, native_func f)
{
	int symbol = symbols_insert(name);
	native_chunk* n = sinavm_new_native(f);
	sinavm_bind(vm, symbol, (chunk_header*) n);	
}

/* push 1, if two top integers have the same value, else push 0 */
void equals(sinavm_data* vm)
{
    
}

/* executes either a block or symbol (second item in ds) if the
 * first item in ds is an integer with value != 0
 */
void _if(sinavm_data* vm)
{
    error_assert(!sinavm_list_empty(vm->ds), "if: too few arguments\n");
    error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
        "if: expected integer\n");
    
    integer_chunk* ic = (integer_chunk*) sinavm_pop_front(vm->ds);
    
    error_assert(!sinavm_list_empty(vm->ds), "if: too few arguments\n");
    chunk_header* ch = sinavm_pop_front(vm->ds);
    
    if (0 != ic->value)
    {
        if (BLOCK_CHUNK == ch->type)
        {
            sinavm_execute_block(vm, (block_chunk*) sinavm_pop_front(vm->ds));
        }
        else if (SYMBOL_CHUNK == ch->type)
        {
            /* symbol must be either bound to block or to native */
            symbol_chunk* sc = (symbol_chunk*) ch;
            sina_interpret_symbol(vm, sc->symbol);
        }
        else
        {
            error_exit("if: expected block or symbol\n");
        }
    }    
}

/* redo the current block (does not alter the cs) */
void loop(sinavm_data* vm)
{
    /* assertion: there is a block on cs, otherwise: how was this
       called?
     */
    block_chunk* bc = (block_chunk*) vm->cs->first->data;
    bc->current = bc->code->first;
    /* stop the interpreter from readjusting bc->current before iteration */
    sinavm_flowcontrol_set(vm);
}

/* pop topmost block of ds and push execute it */
void call(sinavm_data* vm)
{
    error_assert(!sinavm_list_empty(vm->ds), "call: too few arguments\n");
    error_assert(BLOCK_CHUNK == sinavm_type_front(vm->ds),
        "call: expected block\n");
    block_chunk* bc = (block_chunk*) sinavm_pop_front(vm->ds);
    sinavm_execute_block(vm, bc);    
}

/* bind a symbol on the DS to the next chunk on the DS */
void bind(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), "bind: too few arguments\n");	
	error_assert(SYMBOL_CHUNK == sinavm_type_front(vm->ds), 
		"bind: expected symbol\n");
	symbol_chunk* sc = (symbol_chunk*) sinavm_pop_front(vm->ds);	
	int symbol = sc->symbol;

	error_assert(!sinavm_list_empty(vm->ds), "bind: too few arguments\n");
	sinavm_bind(vm, symbol, sinavm_pop_front(vm->ds));
}

/* add the two top numbers in the data stack */
void add(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), "add: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"add: expected integer\n");
	integer_chunk* a = (integer_chunk*) sinavm_pop_front(vm->ds);
	
	error_assert(!sinavm_list_empty(vm->ds), "add: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"add: expected integer\n");
	integer_chunk* b = (integer_chunk*) sinavm_pop_front(vm->ds);

	integer_chunk* c = sinavm_new_int(a->value + b->value);
	sinavm_push_front(vm->ds, c);
}

/* append the chunk on top of the stack to the list
 * second from top
 */
void append(sinavm_data* vm)
{
    error_assert(!sinavm_list_empty(vm->ds), "append: not enough arguments\n");
    chunk_header* data = sinavm_pop_front(vm->ds);
    
    error_assert(!sinavm_list_empty(vm->ds), "append: not enough arguments\n");
    error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
        "append: second argument must be list\n");
    
    list_head_chunk* list = (list_head_chunk*) vm->ds->first->data;
    sinavm_push_back(list, data);
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
