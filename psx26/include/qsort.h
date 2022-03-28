/*
 * File:memory.h
 * memory functions pseudo definition header
 */

#ifndef _QSORT_H
#define _QSORT_H

extern void qsort();


#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef long size_t;           /* result type of the sizeof operator */
#endif

extern void _memcpy();
extern void _swap();

#ifndef NULL
#define NULL 0		       /* null pointer constant */
#endif


#endif  /* _qsort_H */
