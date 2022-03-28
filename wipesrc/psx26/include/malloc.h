/*
 * File:malloc.h
 */

extern void free();
extern void *malloc();
extern void *calloc();
extern void *realloc();


#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef long size_t;           /* result type of the sizeof operator */
#endif

#ifndef NULL
#define NULL 0		       /* null pointer constant */
#endif
