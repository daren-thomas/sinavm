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
#include "sina_allocator.h"
#include <stdio.h>
#include <ctype.h>

/* add a function to the symbol table */
void add_func(sinavm_data* vm, char* symbol, native_func f);

void add(sinavm_data* vm);
void sub(sinavm_data* vm);
void mod(sinavm_data* vm);
void append(sinavm_data* vm);
void swap(sinavm_data* vm);
void drop(sinavm_data* vm);
void roll(sinavm_data* vm);
void bind_symbol(sinavm_data* vm);
void call(sinavm_data* vm);
void loop(sinavm_data* vm);
void _if(sinavm_data* vm);
void _not(sinavm_data* vm);
void equals(sinavm_data* vm);
void _break(sinavm_data* vm);
void _dup(sinavm_data* vm);
void print_string(sinavm_data* vm);
void print_int(sinavm_data* vm);
void read_line(sinavm_data* vm);
void list_is_empty(sinavm_data* vm);
void list_new(sinavm_data* vm);
void list_head(sinavm_data* vm);
void list_prepend(sinavm_data* vm);
void char_is_alpha(sinavm_data* vm);
void char_to_upper(sinavm_data* vm);
void trace_on(sinavm_data* vm);
void trace_off(sinavm_data* vm);

void builtins_add(sinavm_data* vm) {
	add_func(vm, "add", add);
	add_func(vm, "sub", sub);
	add_func(vm, "mod", mod);
	add_func(vm, "swap", swap);
	add_func(vm, "drop", drop);
	add_func(vm, "roll", roll);
	add_func(vm, "append", append);
	add_func(vm, "bind", bind_symbol);
    add_func(vm, "call", call);
    add_func(vm, "loop", loop);
    add_func(vm, "if", _if);
    add_func(vm, "not", _not);
    add_func(vm, "equals", equals);
	add_func(vm, "break", _break);
	add_func(vm, "dup", _dup);
	add_func(vm, "print-string", print_string);
	add_func(vm, "print-int", print_int);
	add_func(vm, "read-line", read_line);
	add_func(vm, "list-is-empty", list_is_empty);
	add_func(vm, "list-new", list_new);
	add_func(vm, "list-head", list_head);
	add_func(vm, "list-prepend", list_prepend);
	add_func(vm, "list-append", append);
	add_func(vm, "char-is-alpha", char_is_alpha);
	add_func(vm, "char-to-upper", char_to_upper);
    add_func(vm, "trace-on", trace_on);
    add_func(vm, "trace-off", trace_off);
}

void add_func(sinavm_data* vm, char* name, native_func f)
{
	int symbol = symbols_insert(name);
	native_chunk* n = sinavm_new_native(f);
	sinavm_bind(vm, symbol, (chunk_header*) n);	
}

/* convert character on top of ds to uppercase */
void char_to_upper(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"char-to-upper: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"char-to-upper: expected integer\n");

	integer_chunk* c = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
    c = sinavm_new_int(toupper(c->value));    
    sinavm_push_front(vm->ds, (chunk_header*) c);
	allocate_pop_register();
}

/* pop integer off ds. if it's an alphabetic character, push
 * 1 onto ds, else push 0
 */
void char_is_alpha(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"char-is-alpha: too few arguments\n");
	int type = sinavm_type_front(vm->ds);
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"char-is-alpha: expected integer\n");

	integer_chunk* c = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	if (isalpha(c->value))
	{
		sinavm_push_front(vm->ds, (chunk_header*) sinavm_new_int(1));
	}
	else
	{
		sinavm_push_front(vm->ds, (chunk_header*) sinavm_new_int(0));
	}
	allocate_pop_register();
}

/* remove the first item from the list on top of the
 * ds and push it onto the ds. It is an error to use
 * list-head on an empty list.
 */
void list_head(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"list-head: too few arguments\n");
	error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
		"list-head: expected list\n");

	list_head_chunk* list = (list_head_chunk*) 
		sinavm_pop_front_to_register( vm->ds);	
    
	error_assert(!sinavm_list_empty(list), /* invalidates list */
		"list-head: empty list\n");
    
	chunk_header* data = sinavm_pop_front_to_register(list);

    allocate_push_register(data);    
	sinavm_push_front(vm->ds, (chunk_header*) list); /* invalidates list and data */
    data = allocate_pop_register();
	sinavm_push_front(vm->ds, data); /* invalidates list and data */
	allocate_pop_register();
	allocate_pop_register();
}

/* push object on top of ds onto front of list second from top */
void list_prepend(sinavm_data*vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"list-prepend: too few arguments\n");
	chunk_header* ch = sinavm_pop_front_to_register(vm->ds);

	error_assert(!sinavm_list_empty(vm->ds),
		"list-prepend: too few arguments\n");
	error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
		"list-prepend: expected list\n");
    
	list_head_chunk* list = (list_head_chunk*) 
		sinavm_pop_front_to_register(vm->ds);    
	list = sinavm_push_front(list, ch); /* invalidates list and ch */
	sinavm_push_front(vm->ds, (chunk_header*) list);
	allocate_pop_register();
	allocate_pop_register();
}

/* push a new list onto ds */
void list_new(sinavm_data* vm)
{
    list_head_chunk* list = sinavm_new_list();
	sinavm_push_front(vm->ds, (chunk_header*) list);
}

/* pop list of top of ds, check if it's empty. if it is,
 * push 1 else push 0 onto ds.
 */
void list_is_empty(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"list-is-empty: too few arguments\n");
	error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
		"list-is-empty: expected list\n");

	list_head_chunk* list = (list_head_chunk*) 
		sinavm_pop_front_to_register(vm->ds);
    integer_chunk* boolval;
	if (sinavm_list_empty(list))
	{
        boolval = sinavm_new_int(1);
	}
	else
	{
        boolval = sinavm_new_int(0);
	}
    sinavm_push_front(vm->ds, (chunk_header*) boolval);
	allocate_pop_register();
}

/* read a line from stdin and push a list of integers
 * representing that line onto the data stack.
 * newline is added to the list.
 */
void read_line(sinavm_data* vm)
{
	list_head_chunk* list = sinavm_new_list();
	int c;
	while (EOF != (c = getchar()))
	{
		allocate_push_register((chunk_header*) list);
		integer_chunk* i = sinavm_new_int(c); /* invalidates list */
		list = (list_head_chunk*) allocate_pop_register();
		
		list = sinavm_push_back(list, (chunk_header*) i);

		if ('\n' == c)
		{
			break;
		}
	}
	sinavm_push_front(vm->ds, (chunk_header*) list);
}

/* print a list of integers as a series of characters,
 */
void print_string(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"print-string: too few arguments\n");
	error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
		"print-string: expected list\n");

	list_head_chunk* list = (list_head_chunk*) 
		sinavm_pop_front_to_register(vm->ds);
	list_node_chunk* node = list->first;
	while (NULL != node)
	{
		error_assert(INTEGER_CHUNK == node->data->type,
			"print-string: expected list of integers\n");

		integer_chunk* c = (integer_chunk*) node->data;
		putchar(c->value);

		node = node->next;
	}
	allocate_pop_register();
}

/* print an integer 
 */
void print_int(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"print-int: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"print-int: expected integer\n");

	integer_chunk* i = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	allocate_pop_register();
}

/* drop topmost chunk from data stack */
void drop(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"drop: too few arguments\n");

	sinavm_pop_front(vm->ds);
}

/* _duplicate top item on stack. since we have a policy of
 * not changing data, _dup can simply push the *same* item
 * onto front of list, as opposed to creating a copy
 */
void _dup(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"_dup: too few arguments\n");

	chunk_header* ch = vm->ds->first->data;
	sinavm_push_front(vm->ds, ch);
}

/* pop current block off cs */
void _break(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->cs),
		"break: code stack empty\n");

	sinavm_pop_front(vm->cs);
}

/* push 1, if two top integers have the same value, else push 0 */
void equals(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"equals: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"equals: expected integer\n");

	integer_chunk* a = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);

	error_assert(!sinavm_list_empty(vm->ds),
		"equals: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"equals: expected integer\n");

	integer_chunk* b = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);

    integer_chunk* boolvalue;
	if (a->value == b->value)
	{
        boolvalue = sinavm_new_int(1);
	}
	else
	{
        boolvalue = sinavm_new_int(0);
	}
    sinavm_push_front(vm->ds, (chunk_header*) boolvalue);
	allocate_pop_register();
	allocate_pop_register();
}

/* executes either a block or symbol (second item in ds) if the
 * first item in ds is an integer with value != 0
 */
void _if(sinavm_data* vm)
{
	/* pop code/symbol to execute conditionally */
    error_assert(!sinavm_list_empty(vm->ds), "if: too few arguments\n");
    chunk_header* ch = sinavm_pop_front_to_register(vm->ds);
   
    /* pop condition */
	error_assert(!sinavm_list_empty(vm->ds), "if: too few arguments\n");
    error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
        "if: expected integer\n");
    
    integer_chunk* ic = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
    
    if (0 != ic->value)
    {
        if (BLOCK_CHUNK == ch->type)
        {
            sinavm_execute_block(vm, (block_chunk*) ch);
        }
        else if (SYMBOL_CHUNK == ch->type)
        {
            /* symbol must be either bound to block or to native */
            sina_interpret_symbol(vm, ((symbol_chunk*) ch)->symbol);
        }
        else
        {
            error_exit("if: expected block or symbol\n");
        }
    }    
	allocate_pop_register();
	allocate_pop_register();
}

/* if top of ds = 0, then push 1, else push 0 */
void _not(sinavm_data* vm)
{
    error_assert(!sinavm_list_empty(vm->ds), "not: too few arguments\n");
    error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
        "not: expected integer\n");
    
    integer_chunk* ic = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	integer_chunk* notic;
	if (0 == ic->value)
	{
		notic = sinavm_new_int(1);	
	}
	else
	{
		notic = sinavm_new_int(0);
	}
    sinavm_push_front(vm->ds, (chunk_header*) notic);
	allocate_pop_register();
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
    block_chunk* bc = (block_chunk*) sinavm_pop_front_to_register(vm->ds);
    sinavm_execute_block(vm, bc);    
	allocate_pop_register();
}

/* bind a symbol on the DS to the next chunk on the DS */
void bind_symbol(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), "bind: too few arguments\n");	
	error_assert(SYMBOL_CHUNK == sinavm_type_front(vm->ds), 
		"bind: expected symbol\n");
	symbol_chunk* sc = (symbol_chunk*) sinavm_pop_front_to_register(vm->ds);	
	int symbol = sc->symbol;
	error_assert(!sinavm_list_empty(vm->ds), "bind: too few arguments\n");
	chunk_header* data = sinavm_pop_front_to_register(vm->ds);
	sinavm_bind(vm, symbol, data);
	allocate_pop_register(); /* symbol */
	allocate_pop_register(); /* data   */
}

/* add the two top numbers in the data stack */
void add(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), 
		"add: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"add: expected integer\n");
	integer_chunk* a = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	
	error_assert(!sinavm_list_empty(vm->ds), 
		"add: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"add: expected integer\n");
	integer_chunk* b = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);

    integer_chunk* c = sinavm_new_int(a->value + b->value);
	sinavm_push_front(vm->ds, (chunk_header*) c);

	allocate_pop_register();
	allocate_pop_register();
}

/* subtract the two top numbers in the data stack */
void sub(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), 
		"sub: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"sub: expected integer\n");
	integer_chunk* b = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	
	error_assert(!sinavm_list_empty(vm->ds), 
		"sub: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"sub: expected integer\n");
	integer_chunk* a = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);

	integer_chunk* c = sinavm_new_int(a->value - b->value);
	sinavm_push_front(vm->ds, (chunk_header*) c);
	allocate_pop_register();
	allocate_pop_register();
}

/* return the modulo of the two top numbers in the data stack */
void mod(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds), 
		"mod: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"mod: expected integer\n");
	integer_chunk* b = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);
	
	error_assert(!sinavm_list_empty(vm->ds), 
		"mod: too few arguments\n");
	error_assert(INTEGER_CHUNK == sinavm_type_front(vm->ds),
		"mod: expected integer\n");
	integer_chunk* a = (integer_chunk*) sinavm_pop_front_to_register(vm->ds);

	integer_chunk* c = sinavm_new_int(a->value % b->value);
	sinavm_push_front(vm->ds, (chunk_header*) c);
	allocate_pop_register();
	allocate_pop_register();
}


/* append the chunk on top of the stack to the list
 * second from top
 */
void append(sinavm_data* vm)
{
    error_assert(!sinavm_list_empty(vm->ds), 
		"append: not enough arguments\n");
    chunk_header* data = sinavm_pop_front_to_register(vm->ds);
    
    error_assert(!sinavm_list_empty(vm->ds),
		"append: not enough arguments\n");
    error_assert(LIST_HEAD_CHUNK == sinavm_type_front(vm->ds),
        "append: second argument must be list\n");
    
    list_head_chunk* list = (list_head_chunk*) vm->ds->first->data;
    sinavm_push_back(list, data);

	allocate_pop_register();
}

/* roll the top three arguments on the stack, so that
 * the sequence (a b c) ends up as (b c a)
 */
void roll(sinavm_data* vm)
{
	error_assert(!sinavm_list_empty(vm->ds),
		"roll: not enough arguments\n");
	chunk_header* a = sinavm_pop_front_to_register(vm->ds);
	
	error_assert(!sinavm_list_empty(vm->ds),
		"roll: not enough arguments\n");
	chunk_header* b = sinavm_pop_front_to_register(vm->ds);

	error_assert(!sinavm_list_empty(vm->ds),
		"roll: not enough arguments\n");
	chunk_header* c = sinavm_pop_front_to_register(vm->ds);

    /* appending a, b and c with push front will invalidate the rest */
    allocate_push_register(b);
    allocate_push_register(c); 
    allocate_push_register(a);
        
	sinavm_push_front(vm->ds, allocate_pop_register());
	sinavm_push_front(vm->ds, allocate_pop_register());
	sinavm_push_front(vm->ds, allocate_pop_register());
	allocate_pop_register();
	allocate_pop_register();
	allocate_pop_register();
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
		chunk_header* a = sinavm_pop_front_to_register(vm->ds);
		

		if (sinavm_list_empty(vm->ds))
		{
			error_exit("swap: too few arguments\n");
		}
		else
		{
			chunk_header* b = sinavm_pop_front_to_register(vm->ds);
			
            allocate_push_register(b);
            allocate_push_register(a);
            
			sinavm_push_front(vm->ds, allocate_pop_register());
			sinavm_push_front(vm->ds, allocate_pop_register());

			allocate_pop_register();
			allocate_pop_register();
		}
	}
}

/* set the trace flag on */
void trace_on(sinavm_data* vm)
{
    sinavm_trace_set(vm);
}

/* turn the trace flag off (default) */
void trace_off(sinavm_data* vm)
{
    sinavm_trace_unset(vm);
}
