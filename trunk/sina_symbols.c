/*
 * sina_symbols.c
 *
 * implements the interface sina_symbols.h. See header file
 * for description of the functions.
 */

#include "sina_symbols.h"
#include "sina_error.h"
#include <search.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char*   name;
    int     id;
} node;

static void* root_name   = NULL;
static void* root_id	 = NULL;
static int lastid   = 0;

/* compare two nodes by symbol name */
int compare_names(const void* a, const void* b)
{
    const node* na = (const node*) a;
    const node* nb = (const node*) b;
    
    return strcmp(na->name, nb->name);
}

int compare_ids(const void* a, const void* b)
{
	const node* na = (const node*) a;
	const node* nb = (const node*) b;
	
	return (na->id > nb->id) - (na->id < nb->id);
}

int symbols_insert(char* symbol)
{   
    node* key = malloc(sizeof(node));
    key->name   = strdup(symbol);
    key->id     = 0;    /* we use this to test if key allready exists. */

    node** result = tsearch(key, &root_name, compare_names);
    key = *result;
     
    if (0 == key->id)
    {
        /* new symbol found, assign id, save lookup id=>name */
        key->id = ++lastid;
		tsearch(key, &root_id, compare_ids);
    }
    return key->id;
}

/* returns the name of a symbol by id */
char* symbols_find(int id)
{
	node* key = malloc(sizeof(node));
	key->name = NULL;
	key->id   = id;

	node** result = tfind(key, &root_id, compare_ids);
	if (NULL == result)
	{
		error_exit("symbols_find: invalid symbol id\n");		
	}
	else
	{
		node* symbol = *result;
		return symbol->name;
	}
}
