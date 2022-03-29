/*
 * File:types.h
 */

#ifndef	_SYS_TYPES_H
#define	_SYS_TYPES_H

/*
 * Basic system types and major/minor device constructing/busting macros.
 */

/* major part of a device */
#define	major(x)	((int)(((unsigned)(x)>>8)&0377))

/* minor part of a device */
#define	minor(x)	((int)((x)&0377))

/* make a device number */
#define	makedev(x,y)	((dev_t)(((x)<<8) | (y)))

typedef	unsigned char	u_char;
typedef	unsigned short	u_short;
typedef	unsigned int	u_int;
typedef	unsigned long	u_long;
typedef	unsigned short	ushort;		/* sys III compat */
#ifdef sony_news
typedef	unsigned int	uint;		/* sys V compat */
typedef	unsigned long	ulong;		/* sys V compat */
#endif /* sony_news */

typedef	struct	_physadr { int r[1]; } *physadr;
typedef	struct	label_t	{
	int	val[12];
} label_t;

typedef	struct	_quad { long val[2]; } quad;
// typedef	long	daddr_t;
typedef	char *	caddr_t;
// typedef	long *	qaddr_t;	/* should be typedef quad * qaddr_t; */
//typedef	u_long	ino_t;
// typedef	long	swblk_t;

typedef	long	time_t;
//typedef	short	dev_t;
// typedef	long	off_t;
// typedef	u_short	uid_t;
// typedef	u_short	gid_t;

// #define	NBBY	8		/* number of bits in a byte */

#endif	/* _SYS_TYPES_H */
