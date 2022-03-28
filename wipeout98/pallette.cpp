#include "\wipesrc\psx26\include\sys\types.h"
#include "\wipesrc\psx26\include\libgte.h"
#include <stdlib.h>
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
