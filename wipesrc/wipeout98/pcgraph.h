#ifndef PCGRAPH_H
#define PCGRAPH_H

#define CRTC_ADDR	0x3d4	/* Base port of the CRT Controller (color) */
#define SEQU_ADDR	0x3c4	/* Base port of the Sequencer */
#define GRAC_ADDR	0x3ce	/* Base port of the Graphics Controller */

typedef struct _clip
{
	int16_t	tx, ty;
	int16_t	bx, by;
	int16_t	w, h;
} CLIP;


extern unsigned actStart, visStart;
extern int32_t	curvidmode;
extern CLIP vidclip;
extern CLIP curclip;


extern uint32_t	_SCAddress;
extern uint32_t	_SCSize;

extern int32_t	count;
extern char			*j_buff;
extern int32_t	Gcount;

//extern CGL_RECT_ST  stScreenRect;                // screen box, used by cglClearBuffer

#endif
