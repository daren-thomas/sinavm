#ifndef SINA_SYMBOLS_H
#define SINA_SYMBOLS_H
/*
 * sina_symbols.h
 *
 * defines the interface for the symbol table.
 * the symboltable is used by the lexer.
 */

/* max number of symbols */
#define SINA_SYMBOLS_MAX 1024

/*
 * adds a symbol to the symbol table and returns a numeric
 * identifier for that symbol. If the symbol was allready
 * in the symbol table, return the same numeric identifier
 * as last time.
 */
int symbols_insert(char* symbol);

/*
 * returns the name of a symbol with a specific id
 */
char* symbols_find(int id);

#endif
