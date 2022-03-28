/**************************************************************************
 *
 * FILE     : Eagle.c
 * FUNCTION	: General Purpose Routines For Accessing The Glint / Eagle Card
 *          : And Some New PSX Conversion Type Primitive Handling
 *
 * AUTHOR	: Jed Adams (C) 1995 
 *
 **************************************************************************/

/**************************************************************
 Some Machine Code Shit For Testing Purposes
 **************************************************************/

extern void SetPal(char,char,char,char);
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

extern void Vbl(void);
#pragma aux Vbl=							\
				"push	dx"					\
   			"push	ax"					\
"vloop001:	 mov	dx,03dah"			\
				"in	al,dx"				\
				"test	al,1000b"			\
				"jnz	vloop001"			\
"vloop002:	 mov	dx,03dah"			\
				"in	al,dx"				\
				"test	al,1000b"			\
				"jz	vloop002"			\
				"pop	ax"					\
				"pop	dx"					\
				modify [dx ax];

/**************************************************************
 Allocate the memory
 **************************************************************/

void AllocMem()
{
		GVert=(CGL_VERTEX2D_ST*)Alloc(NumPol*NumVert,sizeof(CGL_VERTEX2D_ST));
		GCol=(CGL_COLOR_ST*)Alloc(NumPol*4,sizeof(CGL_COLOR_ST));
		GouradPtr=(CGL_PTR*)Alloc(NumPol,sizeof(CGL_PTR));
		RenderStruct=(CGL_RENDERFIX_ST*)Alloc(3,sizeof(CGL_RENDERFIX_ST));
		PanelBuf=(CGL_UINT16*)Alloc(640*408,sizeof(CGL_UINT16));

}

void	FreeMem()
{
				DeAlloc(GVert);
				DeAlloc(GCol);
				DeAlloc(GouradPtr);
				DeAlloc(RenderStruct);
				DeAlloc(PanelBuf);
}


void	CloseDown()
{
				FreeMem();
				cglCloseScreen();
				printf("Screen Closed Successfully\n");
}

void	OpenUp()
{
		stConfig.wCurrentMode         = CGL_M640x350x70Hz;
		cglGetConfig(&stConfig);
		AllocMem();

		ReadTGAPanel("stone.raw");


		TestInitScreen();
		Gcount=0;
}

void	Info()
{
		printf("CGL Version         : %s\n",stConfig.Version);
		printf("Board ID            : %d\n",stConfig.wBoard_ID);
		printf("VRAM                : %dmb\n",(stConfig.dwVRAM_Size/1024)/1024);
		printf("DRAM                : %dmb\n",(stConfig.dwDRAM_Size/1024)/1024);
		printf("Max Colour Format   : %d\n",stConfig.wMaxColorFormat);
	   printf("HardwareSupport     : %lx\n",stConfig.uHardwareSupport.i);   

		printf("\nGLINT Screen Opened Successfully\n\n");

		printf("Colour Mode         : 16Bit \n");
		printf("Dither Mode         : ENABLE\n");
		printf("Gouraud V Space     : 0x%x\n",GVert);
		printf("Gouraud ColourSpace : 0x%x\n",GCol);
		printf("GouradPtrs At       : 0x%x\n",GouradPtr);
		printf("RenderStruct At     : 0x%x\n",RenderStruct);
		printf("Number Of Vertices  : %d\n",NumPol*NumVert);
		printf("Number Of Polygons  : %d\n",NumPol);
		printf("No.Of Verts Per Poly: %d\n",NumVert);
}

int	Random(int	num)
{
	int	x;
		x=32768/num;
		x=rand()/x;
		return(x);
}

/**************************************************************
 Initialise GLINT 300 Screen Mode And Other Stuff 
 **************************************************************/

static void TestInitScreen()
{
	CGL_SCREEN_ST ScreenConfig;
						ScreenConfig.wReference      = CGL_TOP_LEFT;
						ScreenConfig.wMode           = CGL_M640x400x70Hz;
						ScreenConfig.wColorFormat    = CGL_RGB8888;
						ScreenConfig.wColorOrder     = CGL_RGB_MODE;
						ScreenConfig.wBuffers        = CGL_DOUBLE;
						ScreenConfig.wOption         = CGL_D0S0;
						ScreenConfig.dwUB_BufferSize = 0;
						cglInitScreen(&ScreenConfig);
						cglSetDitherMode(CGL_ENABLE);
						SizeScreen(ScrX,ScrY);
}


void	SizeScreen(int width,int height)
{
				if (width>=640)
				{
				width=640;
				ScrX=640;
				}

				if (height>=400)
				{
				height=400;
				ScrY=400;
				}

				if (width<=0)
				{
				width=0;
				ScrX=0;
				}

				if (height<=0)
				{
				height=0;
				ScrY=0;
				}


						Width=width;
						Height=height;

						stScreenRect.wX = (640-width)/2;
						stScreenRect.wY = (400-height)/2;
						stScreenRect.wWidth  = width;
						stScreenRect.wHeight = height;

						cglSetWindowClip(NULL);
					
//						cglClearScreen();

						Panel();
			
						cglSwapBuffer();	 

//						cglClearScreen();

						Panel();

						cglSetWindowClip(&stScreenRect);

	printf("Screen Width = %d\n",ScrX);
	printf("Screen Height = %d\n\n",ScrY);


}


/**************************************************************
 Stuff We Do In The Main Loop 
 **************************************************************/

static void DrawPrimitive()
{
						cglClearBuffer(CGL_FRAME_BUFFER , &stScreenRect);
						SetPrims();
						cglSwapBuffer();	 
}


/**************************************************************
 Tells the GLINT to render the number of polys set by the user
 **************************************************************/

void	SetPrims()
{
						RenderStruct[0].wOperation     = CGL_RENDER;
						RenderStruct[0].dwNoOfVertice  = Gcount*3;
						RenderStruct[0].wPrimitiveType = CGL_TRIANGLE;
						RenderStruct[0].uPropertyEnableMask.i = 0x0000;
						RenderStruct[0].uPropertyEnableMask.u.shade = 1;
						RenderStruct[0].pDepth              = NULL;
						RenderStruct[0].pStencil            = NULL;
						RenderStruct[0].pBlend              = NULL;
						RenderStruct[0].pTextureEnv         = NULL;
						RenderStruct[0].pVertex             = &GVert[0];
						RenderStruct[0].pTexture            = NULL;
						RenderStruct[0].pColor              = &GCol[0];
						RenderStruct[0].pDepthCue           = NULL;
						GouradPtr[0] = &RenderStruct[0];
						cglSendStream(&GouradPtr[0],1);
}


void	Panel()
{
						stDownLoad.wBufferType   = CGL_FRAME_BUFFER;
						stDownLoad.wColorFormat  = CGL_RGB5551;
						stDownLoad.pHostBuffer   = PanelBuf+16;
						stDownLoad.stBox.wWidth  = 64;
						stDownLoad.stBox.wHeight = 64;   
						stDownLoad.stBox.wX      = 0;
					   stDownLoad.stBox.wY      = 0;
						cglDownLoad(&stDownLoad);    // download buffer   
}



/**********************************************************
 **
 ** Tell Eagle How To Draw Like The Playstation Does ??????
 **
 ** (C) 1995 Pignoses Ltd.
 **
 **********************************************************/

/**********************************************************
 **														  x1
 ** Draw One Line Flat                        / 
 **													  x0
 **********************************************************/

FlatLine_2(u_long *primitive)
{
LINE_F2	*poly;
	poly = (LINE_F2 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
}


/**********************************************************
 **														  x1
 ** Draw One Line Gouraud'd                   / 
 **													  x0
 **********************************************************/

GouraudLine_2(u_long *primitive)
{
LINE_G2	*poly;
	poly = (LINE_G2 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
}

/**********************************************************
 **														   x1_
 ** Draw Two Lines Connected Together By X1	 /	    - x2
 **													  x0
 **********************************************************/

FlatLine_3(u_long *primitive)
{
LINE_F3	*poly;
	poly = (LINE_F3 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
}


/**********************************************************
 **														   x1_
 ** Draw Two Lines Connected Together By X1	 /	    - x2 & Gouraud Em
 **													  x0
 **********************************************************/

GouraudLine_3(u_long *primitive)
{
LINE_G3	*poly;
	poly = (LINE_G3 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r2;GCol[2+(Gcount*3)].bGreen=poly->g2;GCol[2+(Gcount*3)].bBlue=poly->b2;
Gcount++;
}



/**********************************************************
 **														   	  x1--x2
 ** Draw Three Lines Connected Together By X1 & X2	 /	     \ 
 **													        x0			x3
 **********************************************************/

FlatLine_4(u_long *primitive)
{
LINE_F4	*poly;
	poly = (LINE_F4 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

		GVert[0+(Gcount*3)].x=poly->x3;GVert[0+(Gcount*3)].y=poly->y3;
		GVert[1+(Gcount*3)].x=poly->x2;GVert[1+(Gcount*3)].y=poly->y2;
		GVert[2+(Gcount*3)].x=poly->x3;GVert[2+(Gcount*3)].y=poly->y3;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

}



/**********************************************************
 **														   	  x1--x2
 ** Draw Three Lines Connected Together By X1 & X2	 /	     \  & Gouraud Em
 **													        x0			x3
 **********************************************************/

GouraudLine_4(u_long *primitive)
{
LINE_G4	*poly;
	poly = (LINE_G4 *)primitive;
		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r2;GCol[2+(Gcount*3)].bGreen=poly->g2;GCol[2+(Gcount*3)].bBlue=poly->b2;
Gcount++;
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x3;GVert[1+(Gcount*3)].y=poly->y3;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r3;GCol[1+(Gcount*3)].bGreen=poly->g3;GCol[1+(Gcount*3)].bBlue=poly->b3;
		GCol[2+(Gcount*3)].bRed=poly->r2;GCol[2+(Gcount*3)].bGreen=poly->g2;GCol[2+(Gcount*3)].bBlue=poly->b2;
Gcount++;
}


/**********************************************************
 **														  x1_
 ** Draw A Triangle                           /#__--x2 
 **													  x0--
 **********************************************************/

FlatTri(u_long *primitive)
{
POLY_F3	*poly;
	poly = (POLY_F3 *)primitive;

		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;

		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;

Gcount++;
}

/**********************************************************
 **														  x1_
 ** Draw A Triangle & Gouraud                 /#__-- x2 
 **													  x0--
 **********************************************************/

GouraudTri(u_long *primitive)
{
POLY_G3	*poly;
	poly = (POLY_G3 *)primitive;

		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;

		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r2;GCol[2+(Gcount*3)].bGreen=poly->g2;GCol[2+(Gcount*3)].bBlue=poly->b2;

Gcount++;
}


/**********************************************************
 **												-
 ** Draw A Quad By Doing 2 Triangles  |/| 
 **												-  
 **********************************************************/

FlatQuad(u_long *primitive)
{
POLY_F4	*poly;
	poly = (POLY_F4 *)primitive;

// First Triangle

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

// Second Triangle
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x3;GVert[2+(Gcount*3)].y=poly->y3;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
}

/**********************************************************
 **												-
 ** Draw A Quad By Doing 2 Triangles  |/| & Gouraud It
 **												-  
 **********************************************************/

GouraudQuad(u_long *primitive)
{
POLY_G4	*poly;
	poly = (POLY_G4 *)primitive;

// First Triangle

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

// Second Triangle
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x3;GVert[2+(Gcount*3)].y=poly->y3;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r3;GCol[2+(Gcount*3)].bGreen=poly->g3;GCol[2+(Gcount*3)].bBlue=poly->b3;
Gcount++;
}





void	Triangle(char G,int x1,int y1
							,int x2,int y2
			 	 		   ,int x3,int y3
							,int r1,int g1,int b1
							,int r2,int g2,int b2
							,int r3,int g3,int b3)
{



x1+=(640-Width)/2;
y1+=(400-Height)/2;
x2+=(640-Width)/2;
y2+=(400-Height)/2;
x3+=(640-Width)/2;
y3+=(400-Height)/2;

		GVert[0+(Gcount*3)].x=x1	;GVert[0+(Gcount*3)].y=y1;
		GVert[1+(Gcount*3)].x=x2	;GVert[1+(Gcount*3)].y=y2;
		GVert[2+(Gcount*3)].x=x3	;GVert[2+(Gcount*3)].y=y3;
	
	if (G!=0)
		{
		GCol[0+(Gcount*3)].bRed=r1;	GCol[0+(Gcount*3)].bGreen=g1;	GCol[0+(Gcount*3)].bBlue=b1;
		GCol[1+(Gcount*3)].bRed=r2;	GCol[1+(Gcount*3)].bGreen=g2;	GCol[1+(Gcount*3)].bBlue=b2;
		GCol[2+(Gcount*3)].bRed=r3;	GCol[2+(Gcount*3)].bGreen=g3;	GCol[2+(Gcount*3)].bBlue=b3;
		}
	else
		{
		GCol[0+(Gcount*3)].bRed=r1;	GCol[0+(Gcount*3)].bGreen=g1;	GCol[0+(Gcount*3)].bBlue=b1;
		GCol[1+(Gcount*3)].bRed=r1;	GCol[1+(Gcount*3)].bGreen=g1;	GCol[1+(Gcount*3)].bBlue=b1;
		GCol[2+(Gcount*3)].bRed=r1;	GCol[2+(Gcount*3)].bGreen=g1;	GCol[2+(Gcount*3)].bBlue=b1;
		}
		Gcount++;
}							

void	QUAD(char G,int x1,int y1
					  ,int x2,int y2
			 	     ,int x3,int y3
					  ,int x4,int y4
				     ,int r1,int g1,int b1
					  ,int r2,int g2,int b2
					  ,int r3,int g3,int b3
					  ,int r4,int g4,int b4)
{

int	tmp;

		if (x1>=x2)
			{
			tmp=x1;	x1=x2;	x2=tmp;
			tmp=y1;	y1=y2;	y2=tmp;

			tmp=r1;	r1=r2;	r2=tmp;
			tmp=g1;	g1=g2;	g2=tmp;
			tmp=b1;	b1=b2;	b2=tmp;
			}

		if (x3>=x4)
			{
			tmp=x3;	x3=x4;	x3=tmp;
			tmp=y3;	y3=y4;	y3=tmp;

			tmp=r3;	r3=r4;	r3=tmp;
			tmp=g3;	g3=g4;	g3=tmp;
			tmp=b3;	b3=b4;	b3=tmp;
			}
	
		if (y3>=y1)
			{
			tmp=x1;	x1=x3;	x3=tmp;
			tmp=y1;	y1=y3;	y3=tmp;

			tmp=r1;	r1=r3;	r3=tmp;
			tmp=g1;	g1=g3;	g3=tmp;
			tmp=b1;	b1=b3;	b3=tmp;
			}

		if (y4>=y2)
			{
			tmp=x2;	x2=x4;	x4=tmp;
			tmp=y2;	y2=y4;	y4=tmp;

			tmp=r2;	r2=r4;	r4=tmp;
			tmp=g2;	g2=g4;	g4=tmp;
			tmp=b2;	b2=b4;	b4=tmp;
			}
		
		Triangle(G, x3,y3,x2,y2,x1,y1, r3,g3,b3 ,r2,g2,b2 ,r1,g1,b1);
		Triangle(G, x3,y3,x2,y2,x4,y4, r3,g3,b3 ,r2,g2,b2 ,r4,g4,b4);
}							

