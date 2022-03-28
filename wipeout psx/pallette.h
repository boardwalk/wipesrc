#define	FADE_LEVEL	32
#define	FFADE_LEVEL	32.0

typedef struct colours_
{
	char	blue;
	char	green;
	char	red;
	char	pad;
} COLOURS;

extern void MakePal332(void);
extern void MakePal2222( void );
extern void MakeFade(char red, char green, char blue);
extern char find_nearest(COLOURS *pal, char r, char g, char b);


extern char fade_buf[256][FADE_LEVEL];

extern char max_pal;


extern void	CopyPal(char *,char,char);
extern int GetPal(char);
extern void SetPal(char,char,char,char);

#ifdef DOS_VERSION
#pragma aux CopyPal=					\
					"mov	ah,al"		\
					"mov	max_pal,bl"		\
"@@CopyPalLoop: mov	al,ah"		\
					"mov	bh,[ecx]"	\
					"mov	bl,[ecx+1]"	\
					"mov	dl,[ecx+2]"	\
					"push	ecx"			\
					"mov	cl,dl"		\
					"sar	bh,2"			\
					"sar	bl,2"			\
					"sar	cl,2"			\
					"mov	dx,03c8h"	\
					"out	dx,al"		\
					"inc	dx"			\
					"mov	al,bh"		\
					"out	dx,al"		\
					"mov	al,bl"		\
					"out	dx,al"		\
					"mov	al,cl"		\
					"out	dx,al"		\
					"pop	ecx"			\
					"add	ecx,3"		\
					"inc	ah"			\
					"cmp	ah,max_pal"		\
					"jne	@@CopyPalLoop"\
					parm	 [ecx] [eax] [ebx]\
					modify [ax bx cx dx];


#pragma aux GetPal= \
		"mov	ah,10h" \
		"mov	al,15h"\
		"int	10h"\
		"mov	ah,dh"\
		"xchg ch,cl"\
		"shl	eax,8"\
		"mov	ax,cx"\
		parm	[bl] \
		value [eax] \
		modify [eax dh cx];


#pragma aux SetPal=							\
				"mov	bh,dl"					\
				"mov	edx,03c8h"				\
				"out	dx,al"					\
				"inc	dx"						\
				"mov	al,cl"					\
				"out	dx,al"					\
				"mov	al,bh"					\
				"out	dx,al"					\
				"mov	al,bl"					\
				"out	dx,al"					\
				parm	 [al] [cl] [dl] [bl]	\
				modify [ax bx cx dx];

#else

#pragma aux GetPal = \
	parm [] \
	modify [];

#pragma aux SetPal = \
	parm [] \
	modify [];
#endif
