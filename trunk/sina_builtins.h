#ifndef SINA_BUILTINS_H
#define SINA_BUILTINS_H

/*
 * sina_builtins.h
 *
 * defines the interface for the builtin (predefined) symbols
 */
#include "sinavm.h"
#include "sina_types.h"

/* adds all the builtins to the sinavm. called during initialization of vm.
 */
void builtins_add(sinavm_data* vm);

#endif
