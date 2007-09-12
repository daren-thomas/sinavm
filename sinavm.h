#ifndef sinavm_H
#define sinavm_H

/*
 * sinavm.h
 *
 * defines the interface to the virtual machine used by the
 * sina interpreter. This includes basic functions that alter
 * the vm state and can be called from the intepreter.
 *
 */

#include "sina_types.h"

 /* datastructure used by the interpreter for the vm
  */
typedef struct {
    list_head_chunk* cs;
    list_head_chunk* ds;
} sinavm_data;

/* initializes a sina_vm structure 
 */
void sinavm_initialize(sinavm_data* vm);


/*
 * create a new list and return a pointer to it.
 */
list_head_chunk* sinavm_new_list();

/*
 * append a new node to the list and have that
 * node point to some data.
 */
list_head_chunk* sinavm_push_back(list_head_chunk* list, void* data);

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

#endif
