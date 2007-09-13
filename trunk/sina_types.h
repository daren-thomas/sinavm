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
	chunk_type type;
	chunk_colour colour;
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

typedef native_func void (*
typedef struct {
    chunk_header header;
    native_func  func;
}

#endif
