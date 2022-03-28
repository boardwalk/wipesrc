#define CRTC_ADDR	0x3d4	/* Base port of the CRT Controller (color) */
#define SEQU_ADDR	0x3c4	/* Base port of the Sequencer */
#define GRAC_ADDR	0x3ce	/* Base port of the Graphics Controller */

typedef struct _clip
{
	short	tx, ty;
	short	bx, by;
	short	w, h;
} CLIP;


extern unsigned actStart, visStart;
extern int	curvidmode;
extern CLIP vidclip;
extern CLIP curclip;


extern unsigned long	_SCAddress;
extern unsigned long	_SCSize;

extern int	count;
extern char			*j_buff;
extern int	Gcount;

//extern CGL_RECT_ST  stScreenRect;                // screen box, used by cglClearBuffer
