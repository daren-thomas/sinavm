%{
#include <stdio.h>
#include <string.h>
#define YYDEBUG 1
#define YYERROR_VERBOSE 1

/* simple error handler - we don't need more at the moment */
void yyerror(const char *str)
{
	fprintf(stderr, "error: %s\n", str);
}

int yywrap()
{
	return 1;
}


%}

/* list of tokens for the sina grammar */
%token INTEGER SYMBOL STRING OPEN_PAREN CLOSE_PAREN OPEN_CURLY CLOSE_CURLY COLON


%%

program: 
	list_elements 
		{ 
			printf("program parsed!\n"); 
		} 
	;

list_elements:
	/* empty */
	| list_elements list_element
	;

list_element:
	INTEGER
	| list
	| block
	| escaped_symbol
	| SYMBOL
	| STRING
	;

list: OPEN_PAREN list_elements CLOSE_PAREN ;
block: OPEN_CURLY list_elements CLOSE_CURLY ;
escaped_symbol: COLON SYMBOL ;

%%

/* start the application: parse stdin */
main()
{
	yydebug = 1;
	yyparse();
}
