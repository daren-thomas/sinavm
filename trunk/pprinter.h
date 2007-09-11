#ifndef PPRINTER_H
#define PPRINTER_H
/*
 * pprinter.h
 *
 * pretty printer for sina data (and code) structures.
 * prints out the data as if it were input to the system.
 */

#include "sina_types.h"

/* determines the type of chunk and pretty prints it */
void pprint(chunk_header* chunk);

#endif