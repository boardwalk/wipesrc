#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "getvram.h"
#include "error.h"

#define _SIZE_T_DEFINED_			// PCwipeout
#include <stdio.h>		// PCwipeout

#define	X			1024
#define	Y			512
#define	HEADER	18


void DumpVram(long no)
{
	char	         name[128];
	RECT	         r;
	int	         i,j;
	uchar	         *buff;
	ushort         a,b,c;
	ushort	      *buffw;

	short          count = 0;

	buff = (uchar *) DAlloc (heap, (X*Y*2) + HEADER + 256) ;

	if ( !buff )
	{
		Error( "getvram.c:: DumpVram(): No memory for vram screen", Warning );
		return;
	}

	r.x=0;
	r.y=0;
	r.w=(X);
	r.h=(Y>>1);

	StoreImage( &r, (ulong*)(buff+HEADER) );
	DrawSync (0) ;

	r.x=0;
	r.y=(Y>>1);
	r.w=(X);
	r.h=(Y>>1);

	StoreImage( &r, (ulong*)( buff + HEADER + (X*Y) - 512 ) );
	DrawSync (0) ;

	buffw=(u_short*)( buff + HEADER );
	for(j=0;j<Y;j++)
	{
		for (i=0;i<X;i++)
		{
			a=*buffw;
			b=(a>>5)&31;
			c=(a>>10)&31;
			a=a&31;
			*buffw = (a<<10)|(b<<5)|c;
			buffw++;
		}
		count++;
	}

	buff[0]=0x00;
	buff[1]=0x00;
	buff[2]=0x02;
	buff[3]=0x00;
	buff[4]=0x00;
	buff[5]=0x00;
	buff[6]=0x00;
	buff[7]=0x00;
	buff[8]=0x00;
	buff[9]=0x00;

	buff[10]=0x00;
	buff[11]=0x00;

	buff[12]=(uchar)(X-1);		// PCwipeout
	buff[13]=(X-1) >> 8;
	buff[14]=(uchar)(Y-1);		// PCwipeout
	buff[15]=(Y-1) >> 8;
	buff[16]=0x0f;
	buff[16]=16;
	buff[17]=0x02|0x20;

	sprintf( name, "wipeout/vram.tga", no );
	DumpFile( name, (char *)buff, HEADER+(X*Y*2) );

	DFree ( heap, ( char* )buff ) ;
}


int DumpFile(char *name, char *buff, long len)
{
	FILE* fd;		// PCwipeout

	fd = fopen(name, "wb");	// PCwipeout

	if (fwrite(buff, 1, len, fd) != len)	// PCwipeout
	{
		return (0);
	}

	fclose(fd);	// PCwipeout
	return (len);
}
