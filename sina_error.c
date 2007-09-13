/*
 * sina_error.c
 *
 * implements the interface described in sina_error.h
 */
 #include "sina_error.h"
 #include <stdlib.h>
 
void error_exit(char* reason)
{
    puts("\nERROR:");
    puts(reason);
    exit(EXIT_FAILURE);
}