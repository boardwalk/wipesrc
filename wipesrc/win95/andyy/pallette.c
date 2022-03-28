#include <sys\types.h>
#include <libgte.h>

#include "pallette.h"

char fade_buf[256][FADE_LEVEL];
char	max_pal;

extern CVECTOR Palette[];

/****************************************************************************
;get vga 256 colour palette
;
;INPUT:		Palette number
;				bl
;
;OUTPUT:	DH = RED
;		CH = BLUE
;		CL = GREEN
;
;****************************************************************************/
#ifndef USE_ATI


void MakePal2222( void )
{
int	xc,yc,cc;
int	r,g,b,i;	
int	a;
int	tr,tg,tb;

	a=0;

	cc=0;
	for (i=0;i<4;i++)
	{
		for (r=0;r<4;r++)
		{
			for (g=0;g<4;g++)
			{
				for (b=0;b<4;b++)
				{
				tr=r*4;
				tg=g*4;
				tb=b*4;
				if (tr!=0) tr+=i*16;
				if (tg!=0) tg+=i*16;
				if (tb!=0) tb+=i*16;

				if ((tr==0) && (tg==0) && (tb==0)) 
					{
				 	tr+=i*16;
				 	tg+=i*16;
				 	tb+=i*16;
					}
				SetPal(cc++,tr,tg,tb);
				}
			}
		}						  
	}
}

#endif //USE_ATI


#ifndef USE_ATI


void	MakePal332(void)
{
int Temp, Temp2, Temp3, fred;

	fred=0;
	for (Temp = 0; Temp < 8; Temp++)
	{
		for (Temp2 = 0; Temp2 < 8; Temp2++)
		{
			for (Temp3 = 0; Temp3 < 4; Temp3++)
			{

#ifdef WIPEOUTPC
				if (Temp3!=0)
					SetPal(fred,Temp * 9,Temp2 * 9, 9+Temp3 * 18);
				else
					SetPal(fred,Temp * 9,Temp2 * 9, 0);
#endif
				fred++;
			}
		}
	}

}
#endif //USE_ATI

#ifndef USE_ATI

extern void MakeFade(char red, char green, char blue)
{
	COLOURS current_pallete[256];
	int	rgb;
	int	loop;
	int	fade_loop;
	float	gred, ggreen, gblue;
	float	rstep, gstep, bstep;


//  Get the current pallete

	for (loop = 0; loop < 256; loop++)
		{
		rgb = GetPal((char)loop);
		current_pallete[loop].red = (rgb & 0xff0000) >> 16;
		current_pallete[loop].green = (rgb & 0xff00) >> 8;
		current_pallete[loop].blue = (rgb & 0xff);
		}

	 for (loop =0; loop < 256; loop++)
		{
		rstep = (float)(current_pallete[loop].red - red) / FFADE_LEVEL;
		gstep = (float)(current_pallete[loop].green - green) / FFADE_LEVEL;
		bstep = (float)(current_pallete[loop].blue - blue) / FFADE_LEVEL;
		gred = (float) red;
		ggreen = (float) green;
		gblue = (float) blue;
		for (fade_loop = 0; fade_loop < FADE_LEVEL; fade_loop++)
			{
			fade_buf[loop][fade_loop] = find_nearest(current_pallete,(char)gred,(char) ggreen, (char)gblue);
			gred += rstep;
			ggreen += gstep;
			gblue += bstep;
			}
		}
}

#endif //USE_ATI



char find_nearest(COLOURS *pal, char r, char g, char b)
{
	int loop;
	int diff = 196608, dthis;
	char dnum = 0;


	for (loop =0; loop < 256; loop++)
		{
		dthis = (((pal->red - r) * (pal->red - r)) +
			((pal->green - g) * (pal->green - g))	+
			((pal->blue - b) * (pal->blue - b)));
		if (dthis < diff)
			{
			diff = dthis;
			dnum = loop;
			}
		pal++;
		}
	return(dnum);
}
char FindNearestColourIndex(CVECTOR *TestCol)
{
	short	Red, Green, Blue;
	short	ColourNum;
	char	BestColour = 0;
	short	Difference;
	short	SmallestDifference = 0x7fff;

	Red = TestCol->r;
	Green = TestCol->g;
	Blue = TestCol->b;

	for(ColourNum = 1; ColourNum < 256; ColourNum++)
	{
		Difference = abs(Palette[ColourNum].r - Red);
		Difference += abs(Palette[ColourNum].g - Green);
		Difference += abs(Palette[ColourNum].b - Blue);

		if(Difference == 0)
		{
			BestColour = ColourNum;
			break;
		}
		else if(Difference < SmallestDifference)
		{
			BestColour = ColourNum;
			SmallestDifference = Difference;
		}
	}

	return(BestColour);
}	
