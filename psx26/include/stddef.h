/*
 * File:stddef.h
 */

#ifndef _STDDEF_H
#define _STDDEF_H


#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef long size_t;           /* result type of the sizeof operator */
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned long wchar_t;		 /* type of a wide character */
#endif

#ifndef _UCHAR_T
#define _UCHAR_T
typedef unsigned char u_char;
#endif

#ifndef _USHORT_T
#define _USHORT_T
typedef unsigned short u_short;
#endif

#ifndef _ULONG_T
#define _ULONG_T
typedef unsigned long u_long;
#endif

#ifndef WEOF
#define WEOF    0xffffffff
#endif

#ifndef NULL
#define NULL 0		       /* null pointer constant */
#endif

#endif /* _STDDEF_H */
