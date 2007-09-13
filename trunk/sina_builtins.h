#ifndef SINA_BUILTINS_H
#define SINA_BUILTINS_H

/*
 * sina_builtins.h
 *
 * defines the interface for the builtin (predefined) symbols
 */

/* adds all the builtins to the sinavm. called during initialization of vm.
 */
void builtins_add(sina_data* vm);

#endif