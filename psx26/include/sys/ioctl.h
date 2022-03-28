/*
 * File:ioctl.h
 */

#ifndef IOCTL_H

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define	EOF	(-1)			/* EOF from getc() */
#endif

/* general */
#define	FIOCNBLOCK	(('f'<<8)|1)	/* set non-blocking io */
#define	FIOCSCAN	(('f'<<8)|2)	/* scan for input */

/* tty and sio */
#define	TIOCRAW		(('t'<<8)|1)	/* disable xon/xoff control */
#define	TIOCFLUSH	(('t'<<8)|2)	/* flush input buffer */
#define	TIOCREOPEN	(('t'<<8)|3)	/* reopen */
#define	TIOCBAUD	(('t'<<8)|4)	/* set baud rate */
#define	TIOCEXIT	(('t'<<8)|5)	/* console interrup */
#define	TIOCDTR		(('t'<<8)|6)	/* control DTR line */
#define	TIOCRTS		(('t'<<8)|7)	/* control RTS line */
#define	TIOCLEN		(('t'<<8)|8)	/* character length */
					/* stop<<16 | char */
					/* stop 0:none 1:1 2:1.5 3:2bit */
					/* char 0:5 1:6 2:7 3:8bit */
#define	TIOCPARITY	(('t'<<8)|9)	/* parity 0:none 1:e 3:o */

/* disk */
#define	DIOFORMAT	(('d'<<8)|1)	/* format */

#endif
