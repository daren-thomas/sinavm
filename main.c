#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include "sinavm.h"
#include "sina_parser.tab.h"
#include "sina_allocator.h"
#include "pprinter.h"
#include "sina_symbols.h"


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
    
    /* allocator needs to be initialized before any calls to it are made */
    allocate_heap(1024); /* for now, we hardwire the size of the heap */

	sinavm_data vm;
	printf("before sinavm_initialize\n");
    sinavm_initialize(&vm);
	printf("after sinavm_initialize\n");
	pprint_vm_state(&vm);

	/* start parsing, result will be a block in the variable
	 * code
	 */
	yydebug = 0;
	if (0 == yyparse()) 
	{
		printf("SUCCESS parsing program!\n");
		pprint((chunk_header*) code);
		printf("\n");
        
        printf("\nrunning interpreter...\n\n");
        
        
        sina_interpret(&vm, code);
		printf("\nfinnished interpreting\n");
		pprint_vm_state(&vm);
		return 0;
	}
	else
	{
		printf("ERROR parsing program!\n");
		return 1;
	}
}
