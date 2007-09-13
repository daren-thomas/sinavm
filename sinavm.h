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

/* maximum amount of symbols that can be bound */
#define SINAVM_MAX_SYMBOLS 1024 

 /* datastructure used by the interpreter for the vm
  */
typedef struct {
    list_head_chunk* cs;
    list_head_chunk* ds;
    chunk_header* bindings[SINAVM_MAX_SYMBOLS];  /* list of stuff symbols are bound to */
} sinavm_data;

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
list_head_chunk* sinavm_push_back(list_head_chunk* list, void* data);

/*
 * append a new node to the front of the list and have that node
 * point to data.
 */
list_head_chunk* sinavm_push_front(list_head_chunk* list, void* data);

/*
 * pop the first item off the list. If the list is empty,
 * do nothing.
 */
list_head_chunk* sinavm_pop_front(list_head_chunk* list);

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
block_chunk* sinavm_new_block(list_head_chunk* list);

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

#endif
