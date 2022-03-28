/*************************************
* LoadAll() function & All Routines
* associated with it
*
* Coded By Barfly
*************************************/

/*************************************
* Includes
*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
char	jmax;

extern void	CopyPal(char *,char,char);

/*************************************
* Defines & Externs
*************************************/

extern	char *VRam;
extern	char *CurBuffer;
extern	char *DepthFadeTble;
extern	void  GSwap(void);

#define	PAL		1
#define	PIC		2
#define	DEBUG	4
#define	DONT_CHECK_SIZE	8
#define	ALLOCATE_MEM		16

char	*Path = "alltext\\";		//AJY for relative paths

typedef struct {
	int	width;
	int	height;
	int	vram_xpos;
	int	vram_ypos;
	unsigned char	*bitmap;
	unsigned char	*palette;
} bm;


/******************************************
* LoadALL() Reads All Textures For Ridgie
* Coded By Barfly
******************************************/

void ReadPCX(char *fname, bm *BMptr,char Flg)
{
	
	FILE		*fp;
	int 		xsize=0,	ysize=0,	vram_xpos=0, vram_ypos=0, Isize=0;
	unsigned char		*ptr, *ptr2;
	unsigned char		PCX_byte, RLE_byte;	// pcx code
	unsigned long 	z=0;
	unsigned char		aflag;
	int fLength;
	if ((Flg&DEBUG)!=0) fprintf(stderr,"Attempting To Open File - %s\n",fname);
	
	fLength = FileLength(fname)-(769+128);
	fp = fopen(fname, "rb");
	
	// initialise stuff
	if ((Flg&DONT_CHECK_SIZE)!=0)
	{
		BMptr->width=0;	
		BMptr->height=0;	
	}
	
	BMptr->vram_xpos=0;
	BMptr->vram_ypos=0;
	
	if (fp == NULL)
	{
		xsize = 0;
		ysize = 0;
		fprintf(stderr,"Could Not Open File - %s\n",fname);
		return;
	}
	else
	{
		//			Read Width & Height
		fseek(fp, 4, SEEK_SET);	// skip header
		fread(&vram_xpos,2,1,fp);
		fread(&vram_ypos,2,1,fp);
		fread(&xsize, 2, 1, fp);
		fread(&ysize, 2, 1, fp);
		xsize++;
		ysize++;
		
		Isize = (xsize * ysize);
		
		//			Error Check
		
		if ((Flg&DONT_CHECK_SIZE)==0)
		{
			aflag=0;
			if ( xsize!=BMptr->width )
			{
				fprintf(stderr,"ERR: incorrect width \nLoad Width =%d\nImage Width =%d\n",xsize,BMptr->width);
				aflag=1;
			}
			
			if ( ysize!=BMptr->height )
			{
				fprintf(stderr,"ERR: incorrect height \nLoad Height =%d\nImage Height =%d\n",ysize,BMptr->height);
				aflag=1;
			}
			if (aflag) exit(0);
		}
		else
		{
			BMptr->width=xsize;	
			BMptr->height=ysize;	
			BMptr->vram_xpos=vram_xpos;
			BMptr->vram_ypos=vram_ypos;
		}
		
		//			Debug Info
		
		if ((Flg&DEBUG)!=0)
		{
			fprintf(stderr,"DEBUG ON:\n");
			fprintf(stderr,"Load Width   =%d\n",xsize);
			fprintf(stderr,"Image Width  =%d\n",BMptr->width);
			fprintf(stderr,"Load Height  =%d\n",ysize);
			fprintf(stderr,"Image Height =%d\n",BMptr->height);
		}
		
		//			Load 256 color PCX palette
		
		fseek(fp, -768, SEEK_END);
		
		//		fread(VGA_pal, 1, 768, fp);
		
		if ((Flg&ALLOCATE_MEM)!=0)
		{
			BMptr->bitmap=(unsigned char *)malloc(BMptr->width*BMptr->height);
			if (BMptr->bitmap==NULL)
			{
				fprintf(stderr,"ERR: can't allocate memory for bitmap");
				return;
			}
			
			BMptr->palette=(unsigned char *)malloc(768);
			if (BMptr->palette==NULL)
			{
				fprintf(stderr,"ERR: can't allocate memory for palette");
				return;
			}
		}
		
		ptr=BMptr->bitmap;
		fread(BMptr->palette, 1, 768, fp);
		
		//		if ((Flg&PAL)!=0)
		//		{
		//			for ( Pcount=0;Pcount<767;Pcount++)
		//			{
		//				BMptr->palette[Pcount]=VGA_pal[Pcount];
		//			}
		//		}
		
		
		//			Skip Junk Data
		
		fseek(fp, 128, SEEK_SET);

		ptr2=ptr+Isize-fLength;
		fread(ptr2,fLength,1,fp);		// buffer read
		
		if ((Flg&PIC)!=0)
		{
			
			//			Decode Picture
			
			while (z < Isize)
			{
				PCX_byte=*ptr2++;
				
				if (PCX_byte < 192)
				{
					*ptr++=PCX_byte;
					z++;
				}
				else
				{
					PCX_byte &= 0x3F;
					RLE_byte = *ptr2++;
					for(int i=0;i<PCX_byte;++i)
						*ptr++=RLE_byte;
					z += PCX_byte;
				}
			}
		}
		fclose(fp);
		return;
	}
}
