#ifndef SINA_ERROR_H
#define SINA_ERROR_H

/* 
 * sina_error.h
 *
 * describes the interface for abnormal program termination
 */
 
void error_exit(char* reason);

/*
 * if condition = 0, exit with errormsg.
 */
void error_assert(int condition, char* errormsg);
 
#endif
