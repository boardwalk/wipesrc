extern	unsigned long Randy(unsigned long);
/*
struct POLY_F3{
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	short	x1,	y1;
	short	x2,	y2;
};				/* Flat Triangle */


struct POLY_FT4{
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	unsigned char	u0, v0;	unsigned short	clut;
	short	x1,	y1;
	unsigned char	u1, v1;	unsigned short	tpage;
	short	x2,	y2;
	unsigned char	u2, v2;	unsigned short	pad1;
	short	x3,	y3;
	unsigned char	u3, v3;	unsigned short	pad2;
};					/* Flat Textured Quadrangle */


#pragma aux SetVMode modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyF3 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyFT4 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux GSwap modify [eax ebx ecx edx edi esi];
#pragma aux Pen parm [eax] [ecx] [edx] [ebx] modify exact [eax ebx ecx edx esi edi];
#pragma aux Randy parm [eax] modify exact [ebx edx];


struct POLY_FT4 TestPoly;

void main(void)
{
	int i,j;
	unsigned short cx0,cx1,cx2,cy0,cy1,cy2;
	SetVMode();

	TestPoly.x0=50;
	TestPoly.y0=35;
	TestPoly.x1=92;
	TestPoly.y1=107;
	TestPoly.x2=109;
	TestPoly.y2=47;


	for (j=0;j<600;j++)
	{
		WaitVbl();
		Pen(0,63,63,63);

		for(i=0;i<200;i++)
		{
			cx0=Randy(319-25);
			cy0=Randy(199-25);

			TestPoly.x0=cx0;
			TestPoly.y0=cy0;

			TestPoly.x1=cx0+10;
			TestPoly.y1=cy0;

			TestPoly.x2=cx0;
			TestPoly.y2=cy0+10;

			TestPoly.x3=cx0+10;
			TestPoly.y3=cy0+10;

			TestPoly.r0=Randy(255);

			DrawPolyFT4(&TestPoly);
		}

		GSwap();
		ClearBuf();

		Pen2(0,0,0,0);

	}


}
