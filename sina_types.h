#ifndef SINA_TYPES_H
#define SINA_TYPES_H

/*
 * sina_types.h
 *
 * defines the types used by the sina interpreter. These are the basic
 * building blocks of sina programs.
 *
 */

#include <stddef.h>
#include "sina_error.h"

#define INTEGER_CHUNK 			1
#define SYMBOL_CHUNK 			2
#define ESCAPED_SYMBOL_CHUNK    3
#define LIST_NODE_CHUNK 		4
#define LIST_HEAD_CHUNK			5
#define BLOCK_CHUNK				6
#define NATIVE_CHUNK            7

typedef char chunk_type;
typedef char chunk_colour; /* free vs. black, grey, white */

typedef struct {
	volatile chunk_type type;
	volatile chunk_colour colour;
} chunk_header;

typedef struct {
	chunk_header header;
	int value;
} integer_chunk;

typedef struct {
	chunk_header header;
	int symbol;
} symbol_chunk;

typedef struct {
	chunk_header header;
	int symbol;
} escaped_symbol_chunk;

typedef struct list_node_chunk {
	chunk_header header;
	chunk_header* data;
	struct list_node_chunk* next;
} list_node_chunk;

typedef struct {
	chunk_header header;
	list_node_chunk* first;
	list_node_chunk* last;
} list_head_chunk;

typedef struct {
	chunk_header header;
	list_head_chunk* code;
	list_node_chunk* current;    
} block_chunk;

/* maximum amount of symbols that can be bound */
#define SINAVM_MAX_SYMBOLS 1024 

/* datastructure used by the interpreter for the vm
 */
typedef struct {
    list_head_chunk* cs;
    list_head_chunk* ds;
    chunk_header** bindings;
    unsigned int flags;    
} sinavm_data;

typedef void (*native_func)(sinavm_data*);

typedef struct {
    chunk_header header;
    native_func  func;
} native_chunk;

/* returns the size of a chunk */
size_t sizeof_chunk(int type);

#endif
