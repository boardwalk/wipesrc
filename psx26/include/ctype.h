/*
 * File:ctype.h
 * character handling macro definitions
 */

#ifndef	_CTYPE_H
#define	_CTYPE_H

#define	_U	0x01	/* upper case letter */
#define	_L	0x02	/* lower case letter */
#define	_N	0x04	/* digit */
#define	_S	0x08	/* space, tab, newline, vertical tab, formfeed, or
				carriage return */
#define _P	0x10	/* punctuation character */
#define _C	0x20	/* control character or delete */
#define _X	0x40	/* hexadecimal digit [0-9a-fA-F]*/
#define	_B	0x80	/* blank (space) */

extern	char	_ctype_[];
extern 	char	toupper(),tolower();

#define	isalpha(c)	((_ctype_+1)[(unsigned char)(c)]&(_U|_L))
#define	isupper(c)	((_ctype_+1)[(unsigned char)(c)]&_U)
#define	islower(c)	((_ctype_+1)[(unsigned char)(c)]&_L)
#define	isdigit(c)	((_ctype_+1)[(unsigned char)(c)]&_N)
#define	isxdigit(c)	((_ctype_+1)[(unsigned char)(c)]&(_X|_N))
#define	isspace(c)	((_ctype_+1)[(unsigned char)(c)]&_S)
#define ispunct(c)	((_ctype_+1)[(unsigned char)(c)]&_P)
#define isalnum(c)	((_ctype_+1)[(unsigned char)(c)]&(_U|_L|_N))
#define isprint(c)	((_ctype_+1)[(unsigned char)(c)]&(_P|_U|_L|_N|_B))
#define isgraph(c)	((_ctype_+1)[(unsigned char)(c)]&(_P|_U|_L|_N))
#define iscntrl(c)	((_ctype_+1)[(unsigned char)(c)]&_C)
#define isascii(c)	((unsigned)(c)<=0x7f)
#define toascii(c)	((unsigned char)(c)&0x7f)
#define _toupper(c)	((unsigned char)(c)-'a'+'A')
#define _tolower(c)	((unsigned char)(c)-'A'+'a')

#endif	// _CTYPE_H

#ifdef	_CTYPE_C
#define	_CTYPE_C

/* @(#)ctype_.c	4.2 (Berkeley) 7/8/83 */

static char _ctype_[] = {
	0,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_S,	_S,	_S,	_S,	_S,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_C,	_C,	_C,	_C,	_C,	_C,	_C,	_C,
	_S|_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_P,	_P,	_P,	_P,	_P,	_P,	_P,	_P,
	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,	_N|_X,
	_N|_X,	_N|_X,	_P,	_P,	_P,	_P,	_P,	_P,
	_P,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U|_X,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_U,	_U,	_U,	_U,	_U,
	_U,	_U,	_U,	_P,	_P,	_P,	_P,	_P,
	_P,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L|_X,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_L,	_L,	_L,	_L,	_L,
	_L,	_L,	_L,	_P,	_P,	_P,	_P,	_C
};
#endif	// _CTYPE_C
