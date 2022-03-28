/*
 * File:assert.h
 */

#ifndef	_ASSERT_H
#define	_ASSERT_H

# ifdef NDEBUG

# define _assert(x)
# define assert(x)

# else

# define _assert(x)	{if (!(x)){fprintf(stderr,"Assertion failed: file \"%s\", line %d\n", __FILE__, __LINE__);exit(1);}}
# define assert(x)	_assert(x)

# endif

#endif	/* _ASSERT_H */
