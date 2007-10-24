/*
 * pprinter.c
 *
 * implements the pretty printer in pprinter.h
 *
 */
#include "pprinter.h"
#include "sina_types.h"
#include <stdio.h>

void pprint_integer(integer_chunk*);
void pprint_symbol(symbol_chunk*);
void pprint_escaped_symbol(escaped_symbol_chunk*);
void pprint_list_head(list_head_chunk*);
void pprint_block(block_chunk*);
void pprint_native(native_chunk*);

void pprint_vm_state(sinavm_data* vm)
{
	chunk_header* chunk = (chunk_header*) vm->ds;
	printf("ds: "); pprint(chunk); printf("\n");
	printf("cs: "); pprint((chunk_header*) vm->cs); printf("\n");

}

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

		case NATIVE_CHUNK:
			pprint_native((native_chunk*) chunk);
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
	printf(" %s ", symbols_find(chunk->symbol));
}

void pprint_escaped_symbol(escaped_symbol_chunk* chunk)
{
	printf(" :%s ", symbols_find(chunk->symbol));
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
	list_node_chunk* node;
	for (node = chunk->current; node != NULL; node = node->next)
	{
		pprint((chunk_header*) node->data);
	}
	printf(" } ");
}

void pprint_native(native_chunk* native)
{
	printf("native<%x>", native->func);
}

void pprint_chunk_info(chunk_header* chunk)
{
	int type = chunk->type;
	int colour = chunk->colour;

	/* HACK: LIST_NODE_CHUNK is big enough to hold them all... */
	list_node_chunk* node = (list_node_chunk*) chunk;

	switch (type)
	{
		case INTEGER_CHUNK:
		case SYMBOL_CHUNK:
		case ESCAPED_SYMBOL_CHUNK:
		case NATIVE_CHUNK:
			printf("<t=%d c=%d v=%d>", type, colour, node->next);
			break;

		case LIST_HEAD_CHUNK:
		case LIST_NODE_CHUNK:
		case BLOCK_CHUNK:
			printf("<t=%d c=%d v1=%x v2=%x>", type, colour, 
				node->next, node->data);

		default:
			printf("pprint_chunk_info: unknown type %d, colour=%d", 
				type, colour);
	}
}
