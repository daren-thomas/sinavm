/*
 * pprinter.c
 *
 * implements the pretty printer in pprinter.h
 *
 */
#include "pprinter.h"
#include <stdio.h>

void pprint_integer(integer_chunk*);
void pprint_symbol(symbol_chunk*);
void pprint_escaped_symbol(escaped_symbol_chunk*);
void pprint_list_head(list_head_chunk*);
void pprint_block(block_chunk*);

void pprint(chunk_header* chunk)
{
	chunk_type type = chunk->type;
	switch (type)
	{
		case INTEGER_CHUNK:
			pprint_integer((integer_chunk*) chunk);
			break;

		case SYMBOL_CHUNK:
			pprint_symbol((symbol_chunk*) chunk);
			break;

		case ESCAPED_SYMBOL_CHUNK:
			pprint_escaped_symbol(
				(escaped_symbol_chunk*) chunk);
			break;

		case LIST_HEAD_CHUNK:
			pprint_list_head((list_head_chunk*) chunk);
			break;

		case BLOCK_CHUNK:
			pprint_block((block_chunk*) chunk);
			break;

		default:
			printf("pprint: Bad chunk type: %d\n", type);
	}
}

void pprint_integer(integer_chunk* chunk)
{
	printf(" %d ", chunk->value);
}

void pprint_symbol(symbol_chunk* chunk)
{
	printf(" symbol<%d> ", chunk->symbol);
}

void pprint_escaped_symbol(escaped_symbol_chunk* chunk)
{
	printf(" :symbol<%d> ", chunk->symbol);
}

void pprint_list_head(list_head_chunk* chunk)
{
	printf(" ( ");
	list_node_chunk* node;
	for (node = chunk->first; node != NULL; node = node->next)
	{
		pprint((chunk_header*) node->data);
	}
	printf(" ) ");
}

void pprint_block(block_chunk* chunk)
{
	printf(" { ");
	list_head_chunk* list = chunk->code;
	list_node_chunk* node;
	for (node = list->first; node != NULL; node = node->next)
	{
		pprint((chunk_header*) node->data);
	}
	printf(" } ");
}
