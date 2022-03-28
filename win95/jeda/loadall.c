
/*************************************
 * LoadAll() function & All Routines
 * associated with it
 *
 * Coded By Barfly
 *************************************/

/*************************************
 * Includes
 *************************************/

#include "w:\h\stdio.h"

char	jmax;

extern void	CopyPal(char *,char,char);

#ifdef DOS_VERSION

#pragma aux CopyPal=					\
					"mov	ah,al"		\
					"mov	jmax,bl"	\
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
					"cmp	ah,jmax"	\
					"jne	@@CopyPalLoop"\
					parm	 [ecx] [eax] [ebx]\
					modify [ax bx cx dx];

#endif
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
	char	*bitmap;
	char	*palette;
} bm;


/******************************************
 * LoadALL() Reads All Textures For Ridgie
 * Coded By Barfly
 ******************************************/
#ifndef USE_ATI


void LoadALL()
{
char		NewFilename[256];
int 		Number;
bm			BM;
char		*ptr;
char		*pall;

	ptr =(char *)CurBuffer;

	pall=(char*)malloc(768);

	BM.width=320;
	BM.height=200;
	BM.bitmap=(char*)CurBuffer;
	BM.palette=pall;

	sprintf(NewFilename,"%stitle.pcx",Path);
											/** Read Logo And Display It        **/
	ReadPCX(NewFilename,&BM,PIC|PAL);
	CopyPal(pall,0,255);
	GSwap();
											/** Read Textures                   **/
	BM.width=256;
	BM.height=256;
	BM.palette=pall;

	for (Number=0; Number < 38; Number++)
	{
		sprintf(NewFilename,"%sp%03d.pcx",Path,Number);
		BM.bitmap=(char *)&VRam+(Number*(256*256));
		ReadPCX(NewFilename,&BM,PIC);
	}

	for (Number = 38; Number < 43; Number++)
	{
		sprintf(NewFilename,"%sp%03d.pcx",Path,Number);
		BM.bitmap=(char *)&VRam+((Number - 11)*(256*256));
		ReadPCX(NewFilename,&BM,PIC);
	}
	CopyPal(pall,0,255);
	BM.width=256;
	BM.height=32;
	BM.bitmap=(char *)&DepthFadeTble;
	BM.palette=pall;
	sprintf(NewFilename,"%sdpqnrm.pcx",Path);
	ReadPCX(NewFilename,&BM,PIC);

	free(pall);
}

#endif //USE_ATI


#ifndef USE_ATI

void LoadMenuPages(void)
{
	char		NewFilename[256];
	int 		Number;
	bm			BM;
	char		*ptr;
	char		*pall;


	pall=(char*)malloc(768);

	BM.width=256;
	BM.height=256;
	BM.palette=pall;

	for (Number = 27; Number < 32; Number++)
	{
		sprintf(NewFilename,"%sp%03d.pcx",Path,Number);
		BM.bitmap=(char *)&VRam+((Number - 10)*(256*256));
		ReadPCX(NewFilename,&BM,PIC);
	}

	free(pall);
}
#endif //USE_ATI



#ifndef USE_ATI

void LoadGamePages(void)
{
	char		NewFilename[256];
	int 		Number;
	bm			BM;
	char		*ptr;
	char		*pall;


	pall=(char*)malloc(768);

	BM.width=256;
	BM.height=256;
	BM.palette=pall;

	for (Number = 17; Number < 22; Number++)
	{
		sprintf(NewFilename,"%sp%03d.pcx",Path,Number);
		BM.bitmap=(char *)&VRam+(Number*(256*256));
		ReadPCX(NewFilename,&BM,PIC);
	}

	free(pall);
}
#endif //USE_ATI


ReadPCX(char *fname, bm *BMptr,char Flg)
{

FILE		*fp;
int 		xsize=0,	ysize=0,	vram_xpos=0, vram_ypos=0, Isize=0;
char		*ptr;
char		VGA_pal[768];				// pallette
char		PCX_byte, RLE_byte;	// pcx code
char		r,g,b,Pcount;
unsigned long 	z=0;
char		aflag;

	if ((Flg&DEBUG)!=0) fprintf(stderr,"Attempting To Open File - %s\n",fname);

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
			BMptr->bitmap=(char *)malloc(BMptr->width*BMptr->height);
			if (BMptr->bitmap==NULL)
			{
				fprintf(stderr,"ERR: can't allocate memory for bitmap");
				return;
			}
			
			BMptr->palette=(char *)malloc(768);
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

		if ((Flg&PIC)!=0)
		{

//			Decode Picture

			while (z < Isize)
			{
				fread(&PCX_byte, 1, 1, fp);

				if (PCX_byte < 192)
 			 	{
 			 	  	*ptr++=PCX_byte;
			 		z++;
	 		   }
 			 	else
 			 	{
				PCX_byte = PCX_byte & 0x3F;
      	    	fread(&RLE_byte, 1, 1, fp);
   	       		memset(ptr, RLE_byte, PCX_byte);
         		ptr += PCX_byte;
		       	z += PCX_byte;
          	}
        	}
		}
	fclose(fp);
	return;
	}
}

/*************************************
 * ReadPCX() The Wonderfull Read Pcx
 * Coded By Barfly
 *************************************/
/*
ReadPCX(char *fname, bm *BMptr,char Flg)
{

FILE		*fp;
int			xsize=0,	ysize=0,	Isize=0;
char		*ptr;
char		VGA_pal[768];				// pallette
char		PCX_byte, RLE_byte;	// pcx code
char		r,g,b,Pcount;
unsigned long 	z=0;

	if ((Flg&DEBUG)!=0) fprintf(stderr,"Attempting To Open File - %s\n",fname);

	fp = fopen(fname, "rb");

	if (fp == NULL)
	{
		xsize = 0;
		ysize = 0;
		fprintf(stderr,"Could Not Open File - %s\n",fname);
		return;
	}
	else
	{
		ptr=BMptr->bitmap;

//			Read Width & Height
		fseek(fp, 8, SEEK_SET);
		fread(&xsize, 2, 1, fp);
		fread(&ysize, 2, 1, fp);
		xsize++;
		ysize++;

		Isize = (xsize * ysize);

//			Error Check

		if ( xsize!=BMptr->width )
		{
			fprintf(stderr,"ERR: incorrect width \nLoad Width =%d\nImage Width =%d\n",xsize,BMptr->width);
			return;
		}

//		if ( ysize!=BMptr->height )
//		{
//			fprintf(stderr,"ERR: incorrect height \nLoad Height =%d\nImage Height =%d\n",ysize,BMptr->height);
//			return;
//		}

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

		if ((Flg&PIC)!=0)
		{

//			Decode Picture

			while (z < Isize)
			{
				fread(&PCX_byte, 1, 1, fp);

				if (PCX_byte < 192)
 			 	{
 			 	  	*ptr++=PCX_byte;
			 		z++;
	 		   }
 			 	else
 			 	{
				PCX_byte = PCX_byte & 0x3F;
      	    	fread(&RLE_byte, 1, 1, fp);
   	       		memset(ptr, RLE_byte, PCX_byte);
         		ptr += PCX_byte;
		       	z += PCX_byte;
          	}
        	}
		}
	fclose(fp);
	return;
	}
}
*/
