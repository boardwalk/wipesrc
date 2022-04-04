/*
 *  (C) Copyright 1993/1994 Sony Corporation,Tokyo,Japan. All Rights Reserved
 *
 * 			libetc.h: Pad Interface
 *
 *$Id: libetc.h,v 1.25 1994/11/10 01:28:41 suzu Exp $
 *
 */
#ifndef LIBETC_H
#define LIBETC_H

#include <stdint.h>

extern int32_t PadIdentifier;
/*
 * PAD I/O (SIO Pad)
 */
#define PADLup     (1<<12)
#define PADLdown   (1<<14)
#define PADLleft   (1<<15)
#define PADLright  (1<<13)
#define PADRup     (1<< 4)
#define PADRdown   (1<< 6)
#define PADRleft   (1<< 7)
#define PADRright  (1<< 5)
#define PADi       (1<< 9)
#define PADj       (1<<10)
#define PADk       (1<< 8)
#define PADl       (1<< 3)
#define PADm       (1<< 1)
#define PADn       (1<< 2)
#define PADo       (1<< 0)
#define PADh       (1<<11)

#define MOUSEleft  (1<<3)
#define MOUSEright (1<<2)

#endif
