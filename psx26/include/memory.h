/*
 * File:memory.h
 * memory functions pseudo definition header
 */

#ifndef _MEMORY_H
#define _MEMORY_H

extern void *memcpy();
extern void *memmove();
extern int   memcmp();
extern void *memchr();
extern void *memset();


#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef long size_t;           /* result type of the sizeof operator */
#endif

#ifndef NULL
#define NULL 0		       /* null pointer constant */
#endif


#endif  /* _MEMORY_H */
