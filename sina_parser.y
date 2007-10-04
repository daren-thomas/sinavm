%{
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "sinavm.h"
#include "pprinter.h"

#define YYDEBUG 1

/* simple error handler - we don't need more at the moment */
void yyerror(const char *str)
{
	fprintf(stderr, "error: %s\n", str);
}

int yywrap()
{
	return 1;
}

/* gets set by parsing the program */
block_chunk* code;

%}

/* list of tokens for the sina grammar */
%token OPEN_PAREN CLOSE_PAREN OPEN_CURLY CLOSE_CURLY COLON
%union {
	int number;
	char* string;
	void* pointer;
}

%token <number> INTEGER SYMBOL 
%token <string> STRING 

%type <pointer> program
%type <pointer> list_element list_elements list block escaped_symbol

%%

program: 
	list_elements 
		{ 
			$$ = sinavm_new_block($1);
			code = $$;
		} 
	;

list_elements:
	/* empty */ { $$ = sinavm_new_list(); }
	| list_elements list_element
		{
			$$ = sinavm_push_back($1, $2);
		}
	;

list_element:
	INTEGER 			{ $$ = sinavm_new_int($1); }
	| list				{ $$ = $1; }
	| block				{ $$ = $1; }
	| escaped_symbol	{ $$ = $1; }
	| SYMBOL			{ $$ = sinavm_new_symbol($1); }
	| STRING			{ $$ = sinavm_new_string($1); }
	;

list: OPEN_PAREN list_elements CLOSE_PAREN 
	{
		$$ = $2;
	}
	;
block: OPEN_CURLY list_elements CLOSE_CURLY 
	{
		$$ = sinavm_new_block($2);
	}
	;
escaped_symbol: COLON SYMBOL 
	{
		$$ = sinavm_new_escaped_symbol($2);
	}
	;

%%
