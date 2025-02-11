#ifndef SINAVM_H
#define SINAVM_H

/*
 * sinavm.h
 *
 * defines the interface to the virtual machine used by the
 * sina interpreter. This includes basic functions that alter
 * the vm state and can be called from the intepreter.
 *
 */
#include "sina_types.h"

/* these hooks into sinavm_push/pop_front could be set by the allocator 
 * for monitoring changes to DS/CS 
 */
extern list_head_chunk* (*sinavm_push_front_hook)(
	list_head_chunk* list, chunk_header* data);
extern list_head_chunk* (*sinavm_push_back_hook)(
	list_head_chunk* list, chunk_header* data);
extern list_head_chunk* (*sinavm_pop_front_to_register_hook)(
	list_head_chunk* list);

/* initializes a sina_vm structure 
 */
void sinavm_initialize(sinavm_data* vm);

/*
 * create a new list and return a pointer to it.
 */
list_head_chunk* sinavm_new_list();

/*
 * append a new node to the end of the list and have that
 * node point to data.
 */
list_head_chunk* sinavm_push_back(list_head_chunk* list, chunk_header* data);

/*
 * append a new node to the front of the list and have that node
 * point to data.
 */
list_head_chunk* sinavm_push_front(list_head_chunk* list, chunk_header* data);

/*
 * pop the first item off the list. This discards the node: if it is not
 * referenced anywhere else, it will be collected and reused. 
 */
void sinavm_pop_front(list_head_chunk* list);

/*
 * pop the first item off the list and return it. The item stays reachable
 * at all times since it is pushed onto the register. Must be popped of
 * the register!
 * If list is empty, returns NULL.
 */
chunk_header* sinavm_pop_front_to_register(list_head_chunk* list);

/* return the type of the first item in the list. If the list is
 * empty, return -1 (invalid type value)
 */
int sinavm_type_front(list_head_chunk* list);

/*
 * allocate space for an integer and return a pointer to it
 */
integer_chunk* sinavm_new_int(int value);

/*
 * allocate space for a symbol and return a pointer to it
 */
symbol_chunk* sinavm_new_symbol(int symbol);

/* create a list of integers for a given string and
 * return a pointer to that list.
  */
list_head_chunk* sinavm_new_string(char* string);

/* create escaped symbol cell and the symbol cell for it.
 * return a pointer to it.
 */
escaped_symbol_chunk* sinavm_new_escaped_symbol(int symbol);

/*
 * allocate a new block chunk and use this list. Set the
 * current node pointer to the first element in the list
 */
block_chunk* sinavm_new_block(list_head_chunk* code);

/*
 * allocate a new native func chunk.
 */
native_chunk* sinavm_new_native(native_func f);

/*
 * looks up the chunk a symbol points to and returns it.
 */
chunk_header* resolve_symbol(int symbol);

/*
 * returns 1 if list is empty, else returns 0.
 */
int sinavm_list_empty(list_head_chunk* list);

/*
 * returns the chunk referenced to by the symbol or NULL if symbol
 * was not bound yet.
 */
chunk_header* sinavm_dereference_symbol(sinavm_data* vm, int symbol);

/*
 * binds a symbol to a chunk so that it can be dereferenced later.
 */
void sinavm_bind(sinavm_data* vm, int symbol, chunk_header* data);

/* execute the block by pushing a copy of it (reinitializes the current-pointer)
 * and pushing it onto the CS
 */
void sinavm_execute_block(sinavm_data* vm, block_chunk* block);

/* return 1, if the flowcontrol flag is set. This bit means, that a flow control
 * command was executed and that the vm is set to the next statement (don't
 * advance the blocks current pointer)
 */
int sinavm_flowcontrol_get(sinavm_data* vm);

/* sets the flowcontrol flag. Used by builtins such as 'loop' and 'break' */
void sinavm_flowcontrol_set(sinavm_data* vm);

/* unsets the flowcontrol flag. Called by the interpreter on reading a set 
 * flowcontrol flag.
 */
void sinavm_flowcontrol_unset(sinavm_data* vm);

/* control the trace flag */
int sinavm_trace_get(sinavm_data* vm);
void sinavm_trace_set(sinavm_data* vm);
void sinavm_trace_unset(sinavm_data* vm);


#endif
