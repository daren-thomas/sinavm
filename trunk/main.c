#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "phelper.h"
#include "pprinter.h"
#include "sina_parser.tab.h"

extern FILE* yyin;
extern int yydebug;
extern block_chunk* code;

/* start the application: parse stdin */
int main(int argc, char** argv)
{
	if (2 != argc) /* first argument is executable name... */
	{
		printf("usage: sina FILE\n");
		return 1;
	}

	char* sourcefile = *++argv;
	yyin = fopen(sourcefile, "r");

	/* start parsing, result will be a block in the variable
	 * code
	 */
	yydebug = 0;
	if (0 == yyparse()) 
	{
		printf("SUCCES parsing program!\n");
		pprint((chunk_header*) code);
		printf("\n");
		return 0;
	}
	else
	{
		printf("ERROR parsing program!\n");
		return 1;
	}
}
