#ifndef SINA_INTERPRETER_H
#define SINE_INTERPRETER_H

#include "sina_types.h"
#include "sinavm.h"

/*
 * sina_interpreter.h
 *
 * Defines the interface for the sina interpreter.
 *
 * The sina interpreter has a "stack" (actually, a sina list)
 * that it holds data on. This is called the data stack (DS).
 * Additionally, the sina interpreter has a "code stack (CS)",
 * which holds a pointer to the current block being executed.
 *
 * While the code stack is not empty, the sina interpreter does
 * the following:
 *   - look up the current list_node_chunk of the block
 *     - if it's NULL, pop the block off the code stack
 *     - else interpret the chunk pointed to by the list_node_chunk
 *       according to the rules below.
 *       Set the current list_node_chunk to the next node in the list.
 *
 * Rules for interpreting chunks (depend on data type of chunk):
 * - INTEGER_CHUNK: push chunk onto DS
 * - LIST_HEAD_CHUNK: push chunk onto DS
 * - BLOCK_CHUNK: push chunk onto CS, set current node to first node
 *   in list.
 * - ESCAPED_SYMBOL_CHUNK: push a SYMBOL_CHUNK onto DS with the same
 *   symbol.
 * - SYMBOL_CHUNK: look up symbol in symbol table.
 *   - if symbol points to a BLOCK_CHUNK: push chunk onto CS, set 
 *     current node to first node in list.
 *   - else: interpret chunk.
 */
 
/* start interpreting according to algorithm described above 
 */
void sina_interpret(sinavm_data* vm, block_chunk* code);

#endif
