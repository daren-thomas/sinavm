/*
 * sina_symbols.c
 *
 * implements the interface sina_symbols.h. See header file
 * for description of the functions.
 */

#include "sina_symbols.h"
#include <search.h>
#include <string.h>

typedef struct {
    char*   name;
    int     id;
} node;

static void* root   = NULL;
static int lastid   = 0;

/* compare two nodes by symbol name */
int compare_nodes(const void* a, const void* b)
{
    const node* na = (const node*) a;
    const node* nb = (const node*) b;
    
    return strcmp(na->name, nb->name);
}

int symbols_insert(char* symbol)
{   
    node* key = malloc(sizeof(node));
    key->name   = strdup(symbol);
    key->id     = 0;    /* we use this to test if key allready exists. */

    node** result = tsearch(key, &root, compare_nodes);
    key = *result;
     
    if (0 == key->id)
    {
        /* new symbol found */
        key->id = ++lastid;
    }
     return key->id;
}
