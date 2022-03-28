#include <sys/types.h>

#ifdef __TYPES_H_INCLUDED
	#include "libetc.h"
	#include "libgte.h"
	#include "libgpu.h"
#endif

#ifdef _SYS_TYPES_H
	#include <libetc.h>
	#include <libgte.h>
	#include <libgpu.h>
#endif

#include "table.h"


#define POLYTYPE POLY_FT4

#define POLYTYPEID setPolyFT4



/*
 * Primitive Buffer
 */

#define OTSIZE		30					/* size of ordering table */
#define MAXOBJ		4000				/* max sprite number */


typedef struct 
{		
	DRAWENV		draw;					/* drawing environment */
	DISPENV		disp;					/* display environment */
	int			id;					/* font ID */
	u_long		ot[OTSIZE];			/* ordering table */
	POLYTYPE		sprt[MAXOBJ];		/* 16x16 fixed-size sprite */

} DB;


MATRIX gMat;				/* global matrix */

#define maxVertex 1024  /* 64 longitude by 64 latitude sphere */
#define maxSide	32
#define shift		5

SVECTOR gVec[maxVertex]; /* original object */
SVECTOR sVec[maxVertex]; /* screen transform */
SVECTOR gRot;
SVECTOR gAcc;
SVECTOR gAccr;
VECTOR  gTrans;


typedef struct ZPrimStruct
{
	long polyIndex;
	struct ZPrimStruct * next;	

} ZPrim;


#define MAXBLOCK 50000L
typedef struct HeapStruct
{
	long bytesleft;
	long nextPtr;
	char heap[MAXBLOCK];
} Heap;

#define MAXHEAP 1
Heap heap[MAXHEAP];

ZPrim * zarray[8192];		/* zarray[0] is split into high word low word which contains the index to the polygon
										i.e index to first point in vertex array
										*/


/*
 * Limitations
 */

#define	FRAME_X		320				/* frame size (320x240) */
#define	FRAME_Y		240
#define 	WALL_X		(FRAME_X-16)	/* reflection point */
#define 	WALL_Y		(FRAME_Y-16)

startVCount()
{
	GsClearVcount(0);
}


/* gets the current Vcount and draws a white horizontal line from x,y to x+10,y */
endVCount( long x )
{
	long     y;
	LINE_F2 ln;

	y = GsGetVcount();
	setXY2( &ln, x,y, x+10, y );
	setRGB0( &ln, 0xff, 0xff, 0xff );
	SetLineF2( &ln );
	DrawPrim( &ln );
}



initHeap()
{
	long i;
	for( i=0; i<MAXHEAP; i++ )
	{
		heap[i].bytesleft = MAXBLOCK;
		heap[i].nextPtr = (long)(heap[i].heap);
	}
}

void * mMalloc( long size )
{
	long   i;
	long   t;

	for( i=0; i<MAXHEAP; i++ )
	{
		if( heap[i].bytesleft > size )
		{
			heap[i].bytesleft -= size;
			t = heap[i].nextPtr;
			heap[i].nextPtr += size;
			return (void*)t;
		}
	}
}






main()
{
	DB	db[2];						/* double buffer */
	char		s[128];	  			/* strings to print */
	DB			*cdb;		  			/* current double buffer */
	int		nobj = 1;  			/* object number */
	u_long	*ot;					/* current OT */
	POLYTYPE	*sp;					/* work */
	long		i, x, y;	  			/* work */
	long		flag;
	long 		tx,ty,tz,p,zav;
	long 		indext1, indext2;
	SVECTOR  * tsvec;
	ZPrim		** prim;
	ZPrim		* tprim;


	PadInit(0);						/* reset PAD */
	ResetGraph(0);					/* reset graphic subsystem (0:cold,1:warm) */
	SetGraphDebug(0);				/* set debug mode (0:off, 1:monitor, 2:dump) */
	InitGeom();


	/* initialize environment for double buffer */
	SetDefDrawEnv(&db[0].draw, 0,   0, 320, 240);
	SetDefDrawEnv(&db[1].draw, 0, 240, 320, 240);
	SetDefDispEnv(&db[0].disp, 0, 240, 320, 240);
	SetDefDispEnv(&db[1].disp, 0, 0,   320, 240);


	/* init font environment */
	FntLoad(960, 256);
	db[0].id = FntOpen(16, 16,     128, 64, 0, 32);
	db[1].id = FntOpen(16, 16+240, 128, 64, 0, 32);




	/* dump DB environment */
	DumpDrawEnv(&db[0].draw); DumpDrawEnv(&db[1].draw);
	DumpDispEnv(&db[0].disp); DumpDispEnv(&db[1].disp);
	


	init_prim(&db[0]);			/* initialize primitive buffers #0 */
	init_prim(&db[1]);			/* initialize primitive buffers #1 */

	initVertex();


	gRot.vx = 0;
	gRot.vy = 0;
	gRot.vz = 0;

	gAcc.vx = 0;
	gAcc.vy = 0;
	gAcc.vz = 0;

	gTrans.vx = 0;
	gTrans.vy = 0;
	gTrans.vz = 0;



	/* display */
	SetDispMask(1);				/* enable to display (0:inhibit, 1:enable) */
	
	while ((nobj = pad_read(nobj)) > 0) 
	{
		if(gAcc.vy>0) gAcc.vy -= 1;
		if(gAcc.vy<0) gAcc.vy += 1;

		if(gAcc.vz>0) gAcc.vz -= 1;
		if(gAcc.vz<0) gAcc.vz += 1;

		if(gAcc.vx>0) gAcc.vx -= 1;
		if(gAcc.vx<0) gAcc.vx += 1;

		if(gAccr.vy>0) gAccr.vy -= 10;
		if(gAccr.vy<0) gAccr.vy += 10;

		if(gAccr.vz>0) gAccr.vz -= 10;
		if(gAccr.vz<0) gAccr.vz += 10;

		if(gAccr.vx>0) gAccr.vx -= 10;
		if(gAccr.vx<0) gAccr.vx += 10;

		gTrans.vx += gAcc.vx;
		gTrans.vy += gAcc.vy;
		gTrans.vz += gAcc.vz;

		gRot.vx += gAccr.vx;
		gRot.vy += gAccr.vy;
		gRot.vz += gAccr.vz;


		cdb  = (cdb==db)? db+1: db;	/* swap double buffer ID */
 		ClearOTag(cdb->ot, OTSIZE);	/* clear ordering table */
		
		RotMatrix( &gRot, &gMat );
		gMat.t[0] = gTrans.vx;
		gMat.t[1] = gTrans.vy;
		gMat.t[2] = gTrans.vz;

		SetRotMatrix( &gMat );		/* set the rotation matrix to be this */
		SetTransMatrix( &gMat );	/* set the translation matrix to be this */
		SetGeomScreen( 300L );		/* set persepctive */
		SetGeomOffset( 160L, 130L );



		for( x=0; x<maxVertex; x++ ) /*... no check for end of array */
			sVec[x].vz = RotTransPers(	&gVec[x], &sVec[x], &p, &flag );

		initHeap(); /* wipe all zbuffer allocations */

		for(prim=&zarray[0]; prim<&zarray[8192]; prim++)
			*prim = 0x00000000L;

		for ( x=0; x<(maxSide-1); x++ ) 
		{
			for ( y=0; y<(maxSide-1); y++ ) 
			{
				indext1 = y<<shift;
				indext2 = (y+1)<<shift;

				/* work out avaerage of 4 points */
				zav = AverageZ4( sVec[indext1+x].vz, sVec[indext1+x+1].vz, sVec[indext2+x].vz, sVec[indext2+x+1].vz );

				if( zav > 8191 )
					zav = 8191;

				if( zav >= 10 )
				{
					tprim = zarray[zav];
					zarray[zav] = mMalloc( sizeof(ZPrim) );
					zarray[zav]->polyIndex = (y<<8) + x;
					zarray[zav]->next = tprim;
				}
			}
		}

		/* update sprites */
		ot = cdb->ot;
		sp = cdb->sprt;

		for ( prim=&zarray[0]; prim<&zarray[8192]; prim++ ) 
		{
			tprim = *prim;
			while( tprim != 0x00000000L )
			{
				x = ( (tprim->polyIndex) & 0x000000ffL ) ;
				y = ( (tprim->polyIndex) & 0x0000ff00L ) >> 8;

				indext1 = y<<shift;
				indext2 = (y+1)<<shift;

				sp->x0 =  sVec[indext1+x].vx;
				sp->y0 =  sVec[indext1+x].vy;
				sp->x1 =  sVec[indext1+x+1].vx;
				sp->y1 =  sVec[indext1+x+1].vy;

				sp->x2 =  sVec[indext2+x].vx;
				sp->y2 =  sVec[indext2+x].vy;
				sp->x3 =  sVec[indext2+x+1].vx;
				sp->y3 =  sVec[indext2+x+1].vy;

				setRGB0(sp,y<<4,x<<4,0);

				AddPrim(ot, sp);						/* apend to OT */

				sp++;
				tprim = tprim->next;
			}
		}

//		DrawSync(0);					/* wait for end of drawing */
//		VSync(0);						/* wait for V-BLNK */

		PutDispEnv(&cdb->disp); 	/* update display environment */
		PutDrawEnv(&cdb->draw); 	/* update drawing environment */

		DrawOTag(cdb->ot);
	}
	PadStop();
	exit(0);
}


























initVertex()
{
	short x,y;
	for( x=0; x<maxSide; x++ )
	{
		for( y=0; y<maxSide; y++ )
		{
			gVec[(y<<shift)+x].vx = tblCOS[ y<<shift ] / 20 ;
			gVec[(y<<shift)+x].vz = tblSIN[ y<<shift ] / 20 ;
			gVec[(y<<shift)+x].vy = (x-(maxSide>>1)) * 10;

			gVec[(y<<shift)+x].vx *= 20;
			gVec[(y<<shift)+x].vy *= 20;
			gVec[(y<<shift)+x].vz *= 20;
		}
	}
}





/*
 * Initialize drawing Primitives
 */

#include "balltex.h"

static init_prim(DB * db)
{
	u_short	clut[32];			/* CLUT entry */
	POLYTYPE	*sp;					/* work */
	int	i;							/* work */
	long x,y;
	int tp;	/* 0=4bit   1=8bit   2=16bit */
	int abr; /* 0=.5f + .5b */
				/* 1= */
				/* 2= */
				/* 3= */

	
	/* inititalize double buffer */
	db->draw.isbg = 1;
	setRGB0(&db->draw, 0, 0, 0);

	tp = 2;
	abr = 0;

	/* load texture pattern and CLUT */			
	db->draw.tpage = LoadTPage(ball16x16x16, tp, abr, 640, 0, 16, 16);
	DumpTPage(db->draw.tpage);
	
	for (i = 0; i < 32; i++) 
	{
		clut[i] = LoadClut(ballcolor[i], 0, 480+i);
	}
	
	/* init sprite */
	sp = db->sprt;
	for( x=0; x<(maxSide-1); x++ )
	{
		for( y=0; y<(maxSide-1); y++ )
		{
			POLYTYPEID(sp);	  	/* set primitve ID */
			SetSemiTrans(sp, 1);
			SetShadeTex(sp, 0);
			setUV4(sp,   0,0,   15,0,   0,15,   15,15);					/* texture point is (0,0) */
			sp->clut = clut[i%32];			/* set CLUT */
			sp->tpage = db->draw.tpage;
			sp++;
		}
	}
}	







/*
 * Read controll-pad
 */

static pad_read(int n)
{
	u_long	padd = PadRead(1);

	if(padd & PADLup)	   gAcc.vy -= 5;
	if(padd & PADLdown)	gAcc.vy += 5;
	if(padd & PADLleft)  gAcc.vx -= 5;
	if(padd & PADLright)	gAcc.vx += 5;
	if(padd & PADn) 		gAcc.vz += 5;
	if(padd & PADl) 		gAcc.vz -= 5;

	if(padd & PADk) 	return(-1);

	if(padd & PADRup)		gAccr.vx -= 40;
	if(padd & PADRdown)	gAccr.vx += 40;
	if(padd & PADRleft)	gAccr.vz -= 40;
	if(padd & PADRright)	gAccr.vz += 40;
	if(padd & PADi) 		gAccr.vy += 40;
	if(padd & PADj) 		gAccr.vy -= 40;

	return(n);
}		














