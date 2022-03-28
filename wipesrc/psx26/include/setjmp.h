/*
 * File:setjmp.h
 * simple non-local-jump for single task environment
 */

#ifndef _SETJMP_H
#define _SETJMP_H

/* jmp_buf indices */
#define	JB_PC		0
#define	JB_SP		1
#define	JB_FP		2
#define	JB_S0		3
#define	JB_S1		4
#define	JB_S2		5
#define	JB_S3		6
#define	JB_S4		7
#define	JB_S5		8
#define	JB_S6		9
#define	JB_S7		10
#define	JB_GP		11

#define	JB_SIZE		12

#ifdef LANGUAGE_C
typedef int jmp_buf[JB_SIZE];
extern int setjmp();
extern void longjmp();
#endif /* LANGUAGE_C */

#endif /* _SETJMP_H */
