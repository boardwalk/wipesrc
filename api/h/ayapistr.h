#ifndef	_AYAPISTR_
#define	_AYAPISTR_

typedef	long	AYERR;	/*error code returns*/

typedef struct	_AYRECT {
	int	x,y;
	int	w,h;
	}AYRECT;		 //16

typedef struct _AYDRAWMODE {
	int	width;
	int	height;
	int	colour_depth;
	int	buffers;
	}AYDRAWMODE;	 //16

typedef struct _AYCAPSFLAG {
	unsigned	int	bit4:1;		 	/*1*/
	unsigned	int	bit8pal:1;		/*2*/
	unsigned	int	bit8332:1;		/*3*/
	unsigned	int	bit16:1;		/*4*/
	unsigned	int	bit24:1;		/*5*/
	unsigned	int	antialias:1;	/*6*/
	unsigned	int	mipmap:1;		/*7*/
	unsigned	int	texfilter:1;	/*8*/
	unsigned	int	chroma:1;		/*9*/
	unsigned	int	alphachan:1;	/*10*/
	unsigned	int	translucency:1;	/*11*/
	unsigned	int	tris:1;			/*12*/
	unsigned	int	quads:1;		/*13*/
	unsigned	int	polys:1;		/*14*/
	unsigned	int	draw3D:1;		/*15*/
	unsigned	int	memory:5;		/*16-20*/

	unsigned	int reserved1:1; 	/*21*/
	unsigned	int reserved2:1; 	/*22*/
	unsigned	int reserved3:1; 	/*23*/
	unsigned	int reserved4:1; 	/*24*/
	unsigned	int reserved5:1; 	/*25*/
	unsigned	int reserved6:1; 	/*26*/
	unsigned	int reserved7:1; 	/*27*/
	unsigned	int reserved8:1; 	/*28*/
	unsigned	int reserved9:1; 	/*29*/
	unsigned	int reserved10:1; 	/*30*/
	unsigned	int reserved11:1; 	/*31*/
	unsigned	int reserved12:1; 	/*32*/
	}AYCAPSFLAG;

typedef struct _AYCAPS {
	int			maxx, maxy;
	int			minx, miny;
	int			max_refresh;
	int			min_refresh;
	int			VRAM;
	AYCAPSFLAG	caps_flag;
	int			card_type;
	} AYCAPS;	//36

typedef struct _AYCOL {
	unsigned	char		r;
	unsigned	char		g;
	unsigned	char		b;
	unsigned	char		a;
	} AYCOL;	//4

typedef struct _AYVERT2 {
	long			x;
	long			y;
	} AYVERT2;	   //8

typedef struct _AYWH {
	unsigned	long			w;
	unsigned	long			h;
	} AYWH;	   //8

typedef struct _AYVERT3 {
	long			x;
	long			y;
	long			z;
	} AYVERT3;	   //12

typedef	struct	_AYTCOORD {
	unsigned	char	u;
	unsigned	char	v;
	} AYTCOORD;	  	//2

typedef	struct	_AYPRIMFLAGS {
	unsigned	short		alias:1;
	unsigned	short		mipmap:1;
	unsigned	short		filter:3;		//S3 Maximum?
	unsigned	short		chroma:1;		//This is not really good enough
	unsigned	short		alpha:2;		//S3 maximum?
	unsigned	short		translucency:2;	//PlayStation style 4 level?
	unsigned	short		texdepth:3;

	unsigned	short		unused:2;
	} AYPRIMFLAGS;	//2

typedef unsigned	short 	AYTEXPAGE;

typedef	unsigned	long 	AYTYPE;	//Can probably pair with len in a short
typedef	unsigned	long	AYLEN;

typedef	void	*AYNEXT;

//at this stage all primitives have 3D coord sets.  This needs reviewing.

typedef	struct	_AYPRIMF1 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	} AYPRIMF1;

typedef	struct	_AYPRIMF2 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	}AYPRIMF2;

typedef	struct	_AYPRIMF3 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	}AYPRIMF3;

typedef	struct	_AYPRIMF4 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYVERT3		vert3;
	}AYPRIMF4;
	
typedef	struct	_AYPRIMG2 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYVERT3		vert0;
	AYVERT3		vert1;
	}AYPRIMG2;

typedef	struct	_AYPRIMG3 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYCOL		col2;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	}AYPRIMG3;

typedef	struct	_AYPRIMG4 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYCOL		col2;
	AYCOL		col3;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYVERT3		vert3;
	}AYPRIMG4;
	

typedef	struct	_AYPRIMFT1 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	} AYPRIMFT1;

typedef	struct	_AYPRIMFT2 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	}AYPRIMFT2;

typedef	struct	_AYPRIMFT3 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	AYTCOORD	t2;
	}AYPRIMFT3;

typedef	struct	_AYPRIMFT4 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYVERT3		vert3;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	AYTCOORD	t2;
	AYTCOORD	t3;
	}AYPRIMFT4;
	
typedef	struct	_AYPRIMGT2 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	}AYPRIMGT2;

typedef	struct	_AYPRIMGT3 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYCOL		col2;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	AYTCOORD	t2;
	}AYPRIMGT3;

typedef	struct	_AYPRIMGT4 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYCOL		col1;
	AYCOL		col2;
	AYCOL		col3;
	AYVERT3		vert0;
	AYVERT3		vert1;
	AYVERT3		vert2;
	AYVERT3		vert3;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	AYTCOORD	t1;
	AYTCOORD	t2;
	AYTCOORD	t3;
	}AYPRIMGT4;
	

typedef	struct	_AYPRIMSP8 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT2		vert0;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	} AYPRIMSP8;

typedef	AYPRIMSP8 AYPRIMSP16;

typedef	struct	_AYPRIMSP {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYVERT2		vert0;
	AYWH		wh;
	AYPRIMFLAGS	flags;
	AYTEXPAGE	tp;
	AYTCOORD	t0;
	} AYPRIMSP;

typedef	struct	_AYPRIMTI1 {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT2		vert0;
	} AYPRIMTI1;

typedef	AYPRIMTI1	AYPRIMTI8;
typedef	AYPRIMTI1	AYPRIMTI16;


typedef	struct	_AYPRIMTI {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	AYVERT2		vert0;
	AYWH		wh;
	} AYPRIMTI;


typedef	struct	_AYPRIMCLS {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYCOL		col0;
	} AYPRIMCLS;

typedef	struct	_AYPRIMCB {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	AYNEXT		addr;
	} AYPRIMCB;


typedef	struct	_AYHEAD {
	AYLEN		length;
	AYTYPE		type;
	AYNEXT		next;
	} AYHEAD;


typedef	union _AYPRIM {
	AYLEN		length;

	AYHEAD		stdhead;

	AYPRIMF1	F1;
	AYPRIMF2	F2;
	AYPRIMG2	G2;
	AYPRIMF3	F3;
	AYPRIMG3	G3;
	AYPRIMF4	F4;
	AYPRIMG4	G4;

	AYPRIMFT1	FT1;
	AYPRIMFT2	FT2;
	AYPRIMGT2	GT2;
	AYPRIMFT3	FT3;
	AYPRIMGT3	GT3;
	AYPRIMFT4	FT4;
	AYPRIMGT4	GT4;

	AYPRIMSP8	SP8;
	AYPRIMSP16	SP16;
	AYPRIMSP	SP;
	
	AYPRIMTI1	TI1;
	AYPRIMTI8	TI8;
	AYPRIMTI16	TI16;
	AYPRIMTI	TI;

	AYPRIMCLS	CLS;

	AYPRIMCB	CB;

	} AYPRIM;


#endif
