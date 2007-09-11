#ifndef PHELPER_H
#define PHELPER_H

/*
 * phelper.h
 *
 * helper functions for the sina parser.
 *
 */

#include "sina_types.h"

/*
 * create a new list and return a pointer to it.
 */
list_head_chunk* phelper_new_list();

/*
 * append a new node to the list and have that
 * node point to some data.
 */
list_head_chunk* phelper_push_back(list_head_chunk* list, void* data);

/*
 * allocate space for an integer and return a pointer to it
 */
integer_chunk* phelper_new_int(int value);

/*
 * allocate space for a symbol and return a pointer to it
 */
symbol_chunk* phelper_new_symbol(int symbol);

/* create a list of integers for a given string and
 * return a pointer to that list.
 */
list_head_chunk* phelper_new_string(char* string);

/* create escaped symbol cell and the symbol cell for it.
 * return a pointer to it.
 */
escaped_symbol_chunk* phelper_new_escaped_symbol(int symbol);

/*
 * allocate a new block chunk and use this list. Set the
 * current node pointer to the first element in the list
 */
block_chunk* phelper_new_block(list_head_chunk* list);

#endif
