/*
 * sina_types.c - implements the sizeof_chunk function
 */

#include "sina_types.h"
#include "sina_error.h"

size_t sizeof_chunk(int type)
{
	switch (type)
	{
		case INTEGER_CHUNK:
			return sizeof(integer_chunk);
			break;

		case SYMBOL_CHUNK:
			return sizeof(symbol_chunk);
			break;

		case ESCAPED_SYMBOL_CHUNK:
			return sizeof(escaped_symbol_chunk);
			break;

		case LIST_NODE_CHUNK:
			return sizeof(list_node_chunk);
			break;

		case LIST_HEAD_CHUNK:
			return sizeof(list_head_chunk);
			break;

		case BLOCK_CHUNK:
			return sizeof(block_chunk);
			break;

		case NATIVE_CHUNK:
			return sizeof(native_chunk);
			break;
		
		default:
			error_exit("sizeof_chunk: unknown type\n");
	}
}
