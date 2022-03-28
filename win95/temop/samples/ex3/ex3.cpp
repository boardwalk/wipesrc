/*==========================================================================
 *
 *  File:       ex3.cpp
 *
 ***************************************************************************/

#define NAME "GTSDK Example 3"
#define TITLE "ATI3DCIF Example 3"

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "resource.h"
#include "ati3dcif.h"
#include "types.h"
#include "exutil.h"
#include "glob.h"
#include "geom.h"
#include "parseasc.h"

#define PI				3.141593f
#define PI_x_2			6.2831853f
#define MAXSCALER		128
#define MINSCALER		1
							    
#define	BACK_BMP		"..\\bmp\\ex3dos.bmp"
#define	BRICK_BMP		"..\\bmp\\texture.bmp"

void CloseApp (void);
void InitView (void);
void updateFrame (void);
void DrawFrame (void);
POBJECT Ex3ObjInit (void);
void UpdateStatusBar (void);

#define CMP(x,y,e) (abs((x)-(y))<(e))

typedef struct
{
	short		steps;
	float		x;		/* position					*/
	float		y;
	float		z;
	float		tx;		/* angle (theta) in radians	*/
	float		ty;
	float		tz;
	float		r;
	float		g;
	float		b;
	short		on;
} SEQPOINT;

typedef struct
{
	short		path;
	short		i;
	short		substep;
	OBJECT*		o;
	float		x;
	float		y;
	float		z;
	float		tx;
	float		ty;
	float		tz;
	float		r;
	float		g;
	float		b;
	float		ix;
	float		iy;
	float		iz;
	float		itx;
	float		ity;
	float		itz;
	float		ir;
	float		ig;
	float		ib;
	short		on;
} DEMOOBJ;

typedef struct
{
	SEQPOINT*	path;
	short		pathlength;
} PATH;

typedef struct
{
	PATH*		p;
	short		paths;
} PATHLIST;

PATHLIST	ReadPathList (void);
SEQPOINT* ReadPathFile( char* name, short* seqpoints );

POBJECT			Ex3Objects		= NULL;	/* the objects to be demoed				*/
short			gNumEx3Objects	= 0;	/* the number of objects to be demoed	*/
DEMOOBJ*		DemoObjs		= NULL;	/* the enapsulation of a demo object	*/
DEMOOBJ**		rDemoObjs		= NULL;	/* objects sorted by depth (z)			*/
PATHLIST		gPathList = {0};

extern VIEW   gView;
extern MATRIX gViewport;
extern int gMapMethod;

HWND			ghWnd;				// window handle
//static char		glpszInstructions [] = " [F1] tex map on/off [F2] shade smooth/flat [F3] tex light on/off [F4] tex filter pick-nearest/bi-linear" ;
char			glpsStatusBar1 [128] = {0};
char			glpsStatusBar2 [128] = {0};

// frame update enable flag
BOOL			gUpdateFrame = FALSE; 

// texture structure for textures
TEXTURE			gTex1 = {0};		
// texture enable flags
BOOL			gbTex1 = FALSE;
BOOL			gbShade = FALSE;
BOOL			gbTexLight = FALSE;
int				gTexFilter = 0;
int 			gSrcAlpha = 0;
int 			gDstAlpha = 0;

// cube transformation matrices
MATRIX			gmatasc;

char lpsTextureMode [64];
char lpsTexLightMode [64];
//char lpsSolidColor [64];
//char lpszgTexPerspCorLevel [64];
char lpsTexFilterMode [64];
char lpsShadeMode [64];
char lpsSrcBlendMode [64];
char lpsDstBlendMode [64];
//char lpszViewPortSize [64];

// texel lighting states
char* alpszTexLightModes [] = {
    "None",
    "Modulate",
    "Alpha decal",
    "not defined"
};

// texture filtering modes
char* alpszTexFilterModes [] = {
    "Pick nearest",
    "Bi-linear",
    "not defined"
};

// perpective correction levels
char* alpszgTexPerspCorLevels [] = {
    "None",
    "One",
    "Two",
    "Three",
    "Four",
    "Five",
    "Six",
    "not defined"
};

// gShade mode states
char* alpszShadeModes [] = {
    "None",
    "Solid",
    "Flat",
    "Smooth",
    "not defined"
};

// alpha blending source factor states
char* alpszSrcAlphaBlendModes [] = {
    "Zero",
    "One",
    "Destination",
    "Inverse destination", 
    "Source alpha",
    "Inverse source alpha",
    "not defined"
};

// alpha blending destination factor states
char* alpszDstAlphaBlendModes [] = {
    "Zero",
    "One",
    "Source",
    "Inverse source",
    "Source alpha",
    "Inverse source alpha",
    "not defined"
};





long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{
    switch( message )
    {
    case WM_CREATE:
	break;

	
    case WM_SETCURSOR:
	SetCursor(NULL);
	return TRUE;

    case WM_KEYDOWN:
	switch( wParam )
	{
			case VK_ESCAPE:
			case VK_F12:
				// disable frame update in WinMain
				gUpdateFrame = FALSE;
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;

			case VK_F1:
				{
					// toggle the texture enable flags
					if (gbTex1) 
						gbTex1 = FALSE;
					else
						gbTex1 = TRUE;

					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_EN, &gbTex1);
				}
				break;

			case VK_F2:
				{
					C3D_ESHADE eshade;

					// toggle the shading mode between flat and gouraud
					if (gbShade)
					{
						// toggle shading from flat to smooth
						gbShade = FALSE;
						eshade = C3D_ESH_FLAT;
					}
					else
					{
						gbShade = TRUE;
						eshade = C3D_ESH_SMOOTH;
					}
					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_SHADE_MODE, &eshade);
					
				}
				break;

			case VK_F3:
				{
					C3D_ETLIGHT eTexLight;

					// toggle the shading mode between flat and gouraud
					if (gbTexLight)
					{
						// if currently on, turn it off
						gbTexLight = FALSE;
						eTexLight = C3D_ETL_NONE;
					}
					else
					{
						// turn it on
						gbTexLight = TRUE;
						eTexLight = C3D_ETL_MODULATE;
					}

					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_LIGHT, &eTexLight);
					
				}
				break;

			case VK_F4:
				{
					C3D_ETEXFILTER eTexFilter;

					// cycle through the texture filtering modes
					gTexFilter++;
					if (gTexFilter >= (int)C3D_ETFILT_NUM) gTexFilter = 0;
					eTexFilter = (C3D_ETEXFILTER) gTexFilter;
					// set source alpha blending mode
					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_FILTER, &eTexFilter);
					
				}
				break;

			case VK_F7:
				{
					C3D_EASRC eASrc;

					// cycle through the source alpha blending modes
					gSrcAlpha++;
					if (gSrcAlpha >= (int)C3D_EASRC_NUM) gSrcAlpha = 0;
					eASrc = (C3D_EASRC) gSrcAlpha;
					// set source alpha blending mode
					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_ALPHA_SRC, &eASrc);
					//wsprintf (lpszSrcBlendMode, "[F7] Source alpha blend mode : %s", alpszSrcAlphaBlendModes [gSrcABlend]);
				}
				break;


			case VK_F8:
				{
					C3D_EADST eADst;

					// cycle through the destination alpha blending modes
					gDstAlpha++;
					if (gDstAlpha >= (int)C3D_EADST_NUM) gDstAlpha = 0;
					eADst = (C3D_EADST) gDstAlpha;
					// set destination alpha blending mode
					ATI3DCIF_ContextSetState (ghRC, C3D_ERS_ALPHA_DST, &eADst);
					//wsprintf (lpszDstBlendMode, "[F8] Dest alpha blend mode : %s", alpszDstAlphaBlendModes [gDstABlend]);
				}
				break;


	} // end switch
	UpdateStatusBar ();
	break;

    case WM_DESTROY:
	CloseApp();
	PostQuitMessage( 0 );
	break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

} // WindowProc

/*
 * InitApp - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL InitApp( HANDLE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
	C3D_EC				ecRetVal;
	C3D_ETLIGHT			eTexLight;
	TEXTMETRIC			tm;
	HDC					hdc;
	int					i;
	PATH*				path;
	SEQPOINT*			point;
	DEMOOBJ*			obj;


    /*
     * set up and register window class
     */
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NAME;
    wc.lpszClassName = NAME;
    RegisterClass( &wc );
    
    /*
     * create a window
     */
    ghWnd = hwnd = CreateWindowEx(
	WS_EX_TOPMOST,
	NAME,
	TITLE,
	WS_POPUP,
	0,
	0,
	GetSystemMetrics( SM_CXSCREEN ),
	GetSystemMetrics( SM_CYSCREEN ),
	NULL,
	NULL,
	hInstance,
	NULL );

    if( !hwnd )
    {
	return FALSE;
    }

    ShowWindow( hwnd, nCmdShow );
    UpdateWindow( hwnd );

    /*
     * create and initialize the main DirectDraw object
     */
	if (!InitDirectDraw(hwnd, 640, 480, 16))
	{
		CloseApp();
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// load the static background bitmap into a plain offscreen surface:
	if (!LoadBackgroundBitmap (BACK_BMP))
	{
		CloseApp();
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// create an ATI CIF redering context:
	if (!InitATI3DCIF ())
	{
		CloseApp();
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// load first texture
	if (!LoadTexture (BRICK_BMP, &gTex1))
	{
		CloseApp();
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// select the texture
	ecRetVal = ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex1.hTX));
	if (ecRetVal != C3D_EC_OK)
	{
		CloseApp();
		wsprintf (gszErrMsg,"Could not select the texture");
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// enable the texture
	gbTex1 = TRUE;
	ecRetVal = ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_EN, &gbTex1);
	if (ecRetVal != C3D_EC_OK)
	{
		CloseApp();
		wsprintf (gszErrMsg,"Could not enable the texture");
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	// enable texture lighting
	eTexLight = C3D_ETL_MODULATE;
	ecRetVal = ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_LIGHT, &eTexLight);
	if (ecRetVal != C3D_EC_OK)
	{
		CloseApp();
		wsprintf (gszErrMsg,"Could not initialize the texture lighting mode");
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	gbShade = TRUE;
	gbTexLight = TRUE;
	gTexFilter = (int)C3D_ETFILT_PICK_NEAREST;
	gSrcAlpha = (int)C3D_EASRC_ONE;
	gDstAlpha = (int)C3D_EADST_ZERO;

	// set texture mapping mode to per-object rather than per-facet
	gMapMethod = MAP_PER_OBJECT;

	// initialize the view parameters
	InitView ();

	Ex3Objects = Ex3ObjInit();
	if (!Ex3Objects)
	{
		CloseApp();
		//wsprintf (gszErrMsg,"Could not initialize the ex3 asc objects");
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}

	for (i = 0; i < gNumEx3Objects; i++)
	{
		ScaleMatrix (3.0f, 3.0f, 3.0f, gmatasc);
		UpdateObject (&Ex3Objects[i], gmatasc);
		TexSetup (&Ex3Objects[i], gmatasc);
	}

	// initialize the gPathList
	gPathList = ReadPathList();

	if (gPathList.paths == 0)
	{
		CloseApp();
		wsprintf (gszErrMsg,"Zero paths");
		MessageBox( hwnd, gszErrMsg, TITLE, MB_OK );
		DestroyWindow( hwnd );
		return FALSE;
	}


	for (i = 0; i < gNumEx3Objects; i++)
	{
		DemoObjs[i].o = &Ex3Objects[i];
		DemoObjs[i].path	= i;
		DemoObjs[i].i       = 0;
		DemoObjs[i].substep = 0;

		obj   = &DemoObjs[i];
		path  = &gPathList.p[obj->path];
		point = &path->path[obj->i];

		DemoObjs[i].x						= point->x;
		DemoObjs[i].y						= point->y;
		DemoObjs[i].z						= point->z;
		DemoObjs[i].tx						= point->tx;
		DemoObjs[i].ty						= point->ty;
		DemoObjs[i].tz						= point->tz;
		DemoObjs[i].r						= point->r;
		DemoObjs[i].g						= point->g;
		DemoObjs[i].b						= point->b;
		DemoObjs[i].ix						=  0.0f;
		DemoObjs[i].iy						=  0.0f;
		DemoObjs[i].iz						=  0.0f;
		DemoObjs[i].itx						=  0.0f;
		DemoObjs[i].ity						=  0.0f;
		DemoObjs[i].itz						=  0.0f;
		DemoObjs[i].ir						=  0.0f;
		DemoObjs[i].ig						=  0.0f;
		DemoObjs[i].ib						=  0.0f;
		DemoObjs[i].o->material.diffuse.x	=  0.0f;
		DemoObjs[i].o->material.diffuse.y	=  0.0f;
		DemoObjs[i].o->material.diffuse.z	=  0.0f;
		DemoObjs[i].o->material.specular.x	=  0.0f;
		DemoObjs[i].o->material.specular.y	=  0.0f;
		DemoObjs[i].o->material.specular.z	=  0.0f;
		DemoObjs[i].o->material.shinyness	= 20.0f;
		DemoObjs[i].o->material.opacity		=  1.0f;
	}

	// 
	UpdateStatusBar ();

	// enable flag to update object geometry 
	gUpdateFrame = TRUE;

    return TRUE;
} // InitApp

/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if( !InitApp( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while( 1 )
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( !GetMessage( &msg, NULL, 0, 0 ) )
			    return msg.wParam;
            TranslateMessage(&msg); 
            DispatchMessage(&msg);
        }

        // update object geomtery
		if (gUpdateFrame) DrawFrame();

    }

} // WinMain


/*
 * InitView - initialize the camera view parameters
 */
void InitView( void )
{
    // location parameters
    gView.camera.location.x =   0.0f;
    gView.camera.location.y =   0.0f;
    gView.camera.location.z = 500.0f;
    gView.camera.upvector.x =   0.0f;
    gView.camera.upvector.y =   1.0f;
    gView.camera.upvector.z =   0.0f;
    gView.camera.lookat.x   =   0.0f;
    gView.camera.lookat.y   =   0.0f;
    gView.camera.lookat.z   =   0.0f;

	// frustum parameters
    gView.frustum.left   =   -3.0f;
    gView.frustum.bottom =   -3.0f;
    gView.frustum.front  =    3.0f;
    gView.frustum.right  =    3.0f;
    gView.frustum.top    =    3.0f;
    gView.frustum.back   = -500.0f;
    gView.frustum.dist   =   10.0f;

    // viewport parameters
    gView.viewport.left   =   0.0f;
    gView.viewport.top    =   0.0f;
    gView.viewport.right  = 480.0f;
    gView.viewport.bottom = 480.0f;

    // light parameters
	gView.num_lights = 2;
    gView.lights[0].light.world_coords.x = 100.0f;
    gView.lights[0].light.world_coords.y = 200.0f;
    gView.lights[0].light.world_coords.z = 1500.0f;
    gView.lights[0].light.ambient.x      = 0.4f;
    gView.lights[0].light.ambient.y      = 0.4f;
    gView.lights[0].light.ambient.z      = 0.4f;
    gView.lights[0].light.diffuse.x      = 0.4f;
    gView.lights[0].light.diffuse.y      = 0.4f;
    gView.lights[0].light.diffuse.z      = 0.4f;
    gView.lights[0].light.specular.x     = 0.4f;
    gView.lights[0].light.specular.y     = 0.4f;
    gView.lights[0].light.specular.z     = 0.4f;

    gView.lights[1].light.world_coords.x = 100.0f;
    gView.lights[1].light.world_coords.y = 1.0f;
    gView.lights[1].light.world_coords.z = 1500.0f;
    gView.lights[1].light.ambient.x      = 0.4f;
    gView.lights[1].light.ambient.y      = 0.4f;
    gView.lights[1].light.ambient.z      = 0.4f;
    gView.lights[1].light.diffuse.x      = 0.4f;
    gView.lights[1].light.diffuse.y      = 0.4f;
    gView.lights[1].light.diffuse.z      = 0.4f;
    gView.lights[1].light.specular.x     = 0.4f;
    gView.lights[1].light.specular.y     = 0.4f;
    gView.lights[1].light.specular.z     = 0.4f;

    // initialize view matrices in gView matrix
    CalcViewParams();
} // InitView




/*
 *  CloseApp - close application components and clean up
 */
void CloseApp (void)
{
	int i;

	// unload first texture
	if (gTex1.lpDDSTex || gTex1.hTX)
	{
		if (!UnloadTexture (&gTex1))
		{
			MessageBox( ghWnd, gszErrMsg, TITLE, MB_OK );
		}
	}
			
	// Destroy the ATI 3D rendering context and close the 3D Driver
	CloseATI3DCIF ();

	// Destroy the DirectDraw object
	CloseDirectDraw ();

	// free object buffers:
/*
	for (i = 0; i < gNumEx3Objects; i++)
	{
		if (DemoObjs[i].o->vertices)
		{
			free (DemoObjs[i].o->vertices);
			DemoObjs[i].o->vertices = NULL;
		}

		if (DemoObjs[i].o->facets)
		{
			free (DemoObjs[i].o->facets);
			DemoObjs[i].o->facets = NULL;
		}

		if (DemoObjs[i].o->facetlist)
		{
			free (DemoObjs[i].o->facetlist);
			DemoObjs[i].o->facetlist = NULL;
		}

		if (DemoObjs[i].o->vlstPrimList)
		{
			free (DemoObjs[i].o->vlstPrimList);
			DemoObjs[i].o->vlstPrimList = NULL;
		}
	}
*/
	if (Ex3Objects)
	{
		for (i = 0; i < gNumEx3Objects; i++)
		{
			if (Ex3Objects[i].vertices)
			{
				free (Ex3Objects[i].vertices);
				Ex3Objects[i].vertices = NULL;
			}

			if (Ex3Objects[i].facets)
			{
				free (Ex3Objects[i].facets);
				Ex3Objects[i].facets = NULL;
			}

			if (Ex3Objects[i].facetlist)
			{
				free (Ex3Objects[i].facetlist);
				Ex3Objects[i].facetlist = NULL;
			}

			if (Ex3Objects[i].vlstPrimList)
			{
				free (Ex3Objects[i].vlstPrimList);
				Ex3Objects[i].vlstPrimList = NULL;
			}
		}

		free (Ex3Objects);
		Ex3Objects = NULL;
	}

	// free the pathlist
	if (gPathList.p) 
	{
		free (gPathList.p);
		gPathList.p = NULL;
	}

	if (DemoObjs)
	{
		free (DemoObjs);
		DemoObjs = NULL;
	}
	if (rDemoObjs)
	{
		free( rDemoObjs );
		rDemoObjs = NULL;
	}

} // CloseApp


/*
 *  updateFrame - update geometry transformation for each object
 */
void updateFrame (void)
{
/*
	MATRIX tran, tmp;
	int i;
	float tx = -4.0f;

	for (i=0; i < gNumEx3Objects; i++)
	{
		TranslateMatrix (tx, 0.0f, 480.0f, tran);
		tx += 1.7f;
		MultMatrix (tran, gmatasc, tmp);
		UpdateObject (&Ex3Objects [i], tmp);
	}
*/

	MATRIX		mat;
	MATRIX		tmp;
	MATRIX		xrot;
	MATRIX		yrot;
	MATRIX		zrot;
	PATH*				path;
	SEQPOINT*			point;
	SEQPOINT*			p;
	DEMOOBJ*			obj;
	int i, j;
	DEMOOBJ*	obj1;
	DEMOOBJ*	obj2;



	for (i = 0; i < gNumEx3Objects; i++)
	{
		obj   = &DemoObjs[i];
		path  = &gPathList.p[obj->path];
		point = &path->path[obj->i];
		// - move the object a substep to its next point in its trajectory												*/
		if ((obj->i < path->pathlength) && (obj->i >= 0))
		{
			obj->o->material.ambient.x  = obj->r;
			obj->o->material.ambient.y  = obj->g;
			obj->o->material.ambient.z  = obj->b;
			obj->o->material.diffuse.x	= obj->r;
			obj->o->material.diffuse.y	= obj->g;
			obj->o->material.diffuse.z	= obj->b;
			obj->o->material.specular.x	= obj->r;
			obj->o->material.specular.y	= obj->g;
			obj->o->material.specular.z	= obj->b;
			obj->o->material.shinyness	= 20.0f;
			obj->o->material.opacity	=  1.0f;

			XRotateMatrix( obj->tx, xrot );
			YRotateMatrix( obj->ty, yrot );
			ZRotateMatrix( obj->tz, zrot );
			MultMatrix( xrot, gmatasc, mat );
			MultMatrix( yrot, mat, tmp );
			MultMatrix( zrot, tmp, mat );
			TranslateMatrix( obj->x, obj->y, obj->z, tmp );
			MultMatrix( tmp, mat, xrot );
			UpdateObject( obj->o, xrot );

			obj->x  += obj->ix;
			obj->y  += obj->iy;
			obj->z  += obj->iz;
			obj->tx += obj->itx;
			obj->ty += obj->ity;
			obj->tz += obj->itz;
			obj->r  += obj->ir;
			obj->g  += obj->ig;
			obj->b  += obj->ib;
			obj->substep++;
		}
		if ((obj->i < path->pathlength) && (obj->i >= 0))
		{
			if (obj->substep >= point->steps)
			{
				obj->i++;
				if (obj->i >= path->pathlength)
				{
					obj->i = 0;
				}
				p = &path->path[obj->i];
				if (p->steps == 0)
				{
					obj->x  = p->x;		/* - if there are no		*/
					obj->y  = p->y;		/*   intervals to the next	*/
					obj->z  = p->z;		/*   point, place the object*/
					obj->tx = p->tx;	/*   there immediately		*/
					obj->ty = p->ty;
					obj->tz = p->tz;
					obj->tz = p->tz;
					obj->r  = p->r;
					obj->g  = p->g;
					obj->b  = p->b;

					obj->o->material.ambient.x  = obj->r;
					obj->o->material.ambient.y  = obj->g;
					obj->o->material.ambient.z  = obj->b;
					obj->o->material.diffuse.x	= obj->r;
					obj->o->material.diffuse.y	= obj->g;
					obj->o->material.diffuse.z	= obj->b;
					obj->o->material.specular.x	= obj->r;
					obj->o->material.specular.y	= obj->g;
					obj->o->material.specular.z	= obj->b;
					obj->o->material.shinyness	= 20.0f;
					obj->o->material.opacity	=  1.0f;

					XRotateMatrix( obj->tx, xrot );
					YRotateMatrix( obj->ty, yrot );
					ZRotateMatrix( obj->tz, zrot );
					MultMatrix( xrot, gmatasc, mat );
					MultMatrix( yrot, mat, tmp );
					MultMatrix( zrot, tmp, mat );
					TranslateMatrix( obj->x, obj->y, obj->z, tmp );
					MultMatrix( tmp, mat, xrot );
					UpdateObject( obj->o, xrot );
				}
				else
				{
					// - adjust the increments for the object's
					//   parameters to slide to the next point
					obj->substep = 0;
					obj->ix  = (p->x  - obj->x)  / (float)p->steps;
					obj->iy  = (p->y  - obj->y)  / (float)p->steps;
					obj->iz  = (p->z  - obj->z)  / (float)p->steps;
					obj->itx = (p->tx - obj->tx) / (float)p->steps;
					obj->ity = (p->ty - obj->ty) / (float)p->steps;
					obj->itz = (p->tz - obj->tz) / (float)p->steps;
					obj->ir  = (p->r  - obj->r)  / (float)p->steps;
					obj->ig  = (p->g  - obj->g)  / (float)p->steps;
					obj->ib  = (p->b  - obj->b)  / (float)p->steps;
					obj->on  = p->on;
				}
			}
		}
		if (obj->i >= path->pathlength)
		{
			/* - move the object back to the beginning of it's		*/
			/*   flight path										*/
			obj->i       = 0;
			obj->substep = 0;
		}

	}

	// place the objects in a new list ordered by the z coordinate	
	for (i = 0; i < gNumEx3Objects; i++)
	{
		rDemoObjs[i] = &DemoObjs[i];
	}

	for (i = 1; i < gNumEx3Objects; i++)
	{
		for (j = i; j < gNumEx3Objects; j++)
		{
			obj1 = rDemoObjs[j];
			obj2 = rDemoObjs[j-1];
			if (obj1->z < obj2->z)
			{
				rDemoObjs[j]   = obj2;
				rDemoObjs[j-1] = obj1;
			}

		}
	}

} // updateFrame


/*
 *  DrawFrame - draw the animation frame and flip the double buffers
 */
void DrawFrame (void)
{
	HRESULT ddretval;
	C3D_EC ecRend;
	int i;
	PATH*				path;
	DEMOOBJ*			obj;

	updateFrame ();

	// blit the static background from the offscreen surface to the back buffer
	ddretval = glpDDSBack->BltFast( 0, 0, glpDDSOne, &grctBkgd, DDBLTFAST_WAIT);
	if (ddretval == DDERR_SURFACELOST) glpDDSBack->Restore ();

	// set the pointer to the rendering buffer for 3D operations
	DDSURFACEDESC ddsd;
	ddsd.dwSize = sizeof (ddsd);
	ddretval = glpDDSBack->Lock (
		NULL,
		&ddsd,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL);
	if (ddretval == DDERR_SURFACELOST) glpDDSBack->Restore ();

	// set the pointer to the frame buffer address of the back surface
	// for the rendering context
	ATI3DCIF_ContextSetState (ghRC, C3D_ERS_SURF_DRAW_PTR, 
		(C3D_PRSDATA) &(ddsd.lpSurface));

	// begin a rendering context
	if ((ecRend = ATI3DCIF_RenderBegin (ghRC)) != C3D_EC_OK) return;
/*
	for (i=0; i < gNumEx3Objects; i++)
	{
		ecRend = ATI3DCIF_RenderPrimList (
			Ex3Objects[i].vlstPrimList,
			Ex3Objects[i].nNumPrimListVerts);
	}
*/

	// render all the objects at once	
	for (i = 0; i < gNumEx3Objects; i++)
	{
		obj = rDemoObjs[i];
		if (obj->on)
		{
			path = &gPathList.p[obj->path];
			if ((obj->i <= path->pathlength) && (obj->i >= 0))
			{
				ATI3DCIF_RenderPrimList (
					obj->o->vlstPrimList, 
					obj->o->nNumPrimListVerts);
			}
		}
	}

	// end the primitive operations
	ecRend = ATI3DCIF_RenderEnd ();

	ddretval = glpDDSBack->Unlock (NULL);
	if (ddretval != DD_OK) return;
	
	while( 1 )
	{
		ddretval = glpDDSPrimary->Flip( NULL, 0 );
		if( ddretval == DD_OK )
		{
			break;
		}

		if( ddretval == DDERR_SURFACELOST )
		{
			ddretval = glpDDSPrimary->Restore();

			if( ddretval != DD_OK )
			{
				break;
			}
		}

		if( ddretval != DDERR_WASSTILLDRAWING )
		{
			break;
		}
	}
}


POBJECT Ex3ObjInit (void)
{
	short		done;
	FILE*		f;
	short		i;
	BOOL		loadstatus;
	char		ascfilename[80];
	short		numobjects	= 0;
	POBJECT pobjects		= NULL;

	/* count the number of ex3dos?.asc files */
	done = FALSE;
	for (i = 0; i < 100 && !done; i++)
	{
		//wsprintf( ascfilename, "ex3dos%1hd.asc", i );
		wsprintf( ascfilename, "ex3dos%d.asc", i );
		f = fopen (ascfilename, "rt");
		if (f == NULL)
		{
			done = TRUE;
		}
		else
		{
			numobjects++;
			fclose (f);
			done = FALSE;
		}
	}
	/* count the number of ex3dos?.asc files */
	gNumEx3Objects = numobjects;	/* set the globals	*/
	if (numobjects > 0)
	{
		pobjects = (POBJECT) malloc (numobjects * sizeof(OBJECT));
		if (pobjects)
		{
			DemoObjs = (DEMOOBJ*) malloc (numobjects * sizeof(DEMOOBJ));
			if (DemoObjs)
			{
				rDemoObjs = (DEMOOBJ**) malloc (numobjects * sizeof(DEMOOBJ*));
				if (rDemoObjs)
				{
/*					for (i = 0; i < numobjects; i++)
					{
						// give the object a default state
						pobjects[i].material.ambient.x	= 0.4f;
						pobjects[i].material.ambient.y	= 0.3f;
						pobjects[i].material.ambient.z	= 0.4f;
						pobjects[i].material.diffuse.x	= 0.0f;
						pobjects[i].material.diffuse.y	= 0.0f;
						pobjects[i].material.diffuse.z	= 0.0f;
						pobjects[i].material.specular.x	= 0.0f;
						pobjects[i].material.specular.y	= 0.0f;
						pobjects[i].material.specular.z	= 0.0f;
						//pobjects[i].material.shinyness	= 1.0f;
						pobjects[i].material.shinyness	= 20.0f;
						pobjects[i].material.opacity	= 1.0f;
					}
*/
					for (i = 0; i < numobjects; i++)
					{
						//wsprintf (ascfilename, "ex3dos%1hd.asc", i);
						wsprintf (ascfilename, "ex3dos%d.asc", i);
						memset (&pobjects[i], 0, sizeof(OBJECT));
						loadstatus = LoadAscObject (&pobjects[i], ascfilename);
						if (!loadstatus)
						{
							MessageBox (ghWnd, gszErrMsg, "", MB_OK);
						}
						pobjects[i].material.ambient.x	= 0.4f;
						pobjects[i].material.ambient.y	= 0.0f;
						pobjects[i].material.ambient.z	= 0.0f;
						pobjects[i].material.diffuse.x	= 0.3f;
						pobjects[i].material.diffuse.y	= 0.0f;
						pobjects[i].material.diffuse.z	= 0.0f;
						pobjects[i].material.specular.x	= 0.4f;
						pobjects[i].material.specular.y	= 0.0f;
						pobjects[i].material.specular.z	= 0.0f;
						//pobjects[i].material.shinyness	= 1.0f;
						pobjects[i].material.shinyness	= 20.0f;
						pobjects[i].material.opacity	= 1.0f;
					}
				}
				else
				{
					free (DemoObjs);
					DemoObjs = NULL;
					free (pobjects);
					pobjects = NULL;
				}
			}
			else
			{
				free (pobjects);
				pobjects = NULL;
			}
		}
	}
	return pobjects;
}




PATHLIST	ReadPathList (void)
{
	short		count = 0;
	BOOL		bDone;
	FILE*		flPatFile;
	short		i;
	char		name[80];
	PATHLIST	p;

	// count the number of path files
	bDone = FALSE;
	for (i = 0; i < 100 && !bDone; i++)
	{
		sprintf (name, "ex3dos%1hd.pat", i);
		flPatFile = fopen (name, "rt");
		if (!flPatFile)
		{
			bDone = TRUE;
		}
		else
		{
			count++;
			fclose (flPatFile);
			bDone = FALSE;
		}
	}
	if (count > 0)
	{
		p.paths = count;
		p.p = (PATH*) malloc (p.paths * sizeof(PATH));
		if (p.p != NULL)
		{
			for (i = 0; i < p.paths; i++)
			{
				sprintf( name, "ex3dos%1hd.pat", i );
				p.p[i].path = ReadPathFile( name, &p.p[i].pathlength );
			}
		}
	}
	return p;
}


SEQPOINT* ReadPathFile( char* name, short* seqpoints )
{
	char		c;
	FILE*		f		= fopen( name, "rt" );
	short		i;
	char		line[1024];
	short		lines	= 0;
	char		onoff[20];
	short		on		= TRUE;
	short		status;
	SEQPOINT*	path	= NULL;
	SEQPOINT	s;
	short		points	= 0;

	if (f != NULL)
	{
		do
		{
			memset( line, 0, sizeof(line) );
			fgets( line, sizeof(line), f );
			if (!feof( f ))
			{
				sscanf( line, " %c ", &c );
				if (c != ';')
				{
					status =	sscanf
								(
									line,
									" %hd %f %f %f %f %f %f ",
									&s.steps,
									&s.x,
									&s.y,
									&s.z,
									&s.tx,
									&s.ty,
									&s.tz
								);
					if (status == 7)	/* - are there at least 7 numbers	*/
					{					/*   on a line in a .pat file?		*/
						points++;
					}
				}
				lines++;
			}
		} while (!feof( f ));
		fclose( f );
		f = fopen( name, "rt" );
		if (seqpoints != NULL)
		{
			*seqpoints = points;
		}
		path = (SEQPOINT*) malloc (points * sizeof(SEQPOINT));
		i = 0;
		while (i < points)
		{
			memset( line, 0, sizeof(line) );
			fgets( line, sizeof(line), f );
			sscanf( line, " %c ", &c );
			if (c != ';')
			{
				status =	sscanf
							(
								line,
								" %hd %f %f %f %f %f %f %f %f %f %s ",
								&s.steps,
								&s.x,
								&s.y,
								&s.z,
								&s.tx,
								&s.ty,
								&s.tz,
								&s.r,
								&s.g,
								&s.b,
								onoff
							);
				if (status < 10)	/* - the RGB info is optional in a .pat	*/
				{					/*   file								*/
					s.r = 1.0f;
					s.g = 1.0f;
					s.b = 1.0f;
				}
				else if (status > 10)
				{
					s.on = on;
					if (stricmp( onoff, "on" ) == 0)
						s.on = TRUE;
					if (stricmp( onoff, "off" ) == 0)
						s.on = FALSE;
				}

				path[i] = s;
				i++;
			}
		}
		fclose( f );
	}
	return path;
}

/*
 *  UpdateStatusBar - update the status bar and redraw it on the background bimtap
 */
void UpdateStatusBar (void)
{
	HDC hdc;
	TEXTMETRIC tm;

	if (gbTex1)
		sprintf (lpsTextureMode, "[F1] texture enabled");
	else
		sprintf (lpsTextureMode, "[F1] texture disabled");

	if (gbShade)
		sprintf (lpsShadeMode, "[F2] shading smooth");
	else
		sprintf (lpsShadeMode, "[F2] shading flat");

	if (gbTexLight)
		sprintf (lpsTexLightMode, "[F3] texture lighted");
	else
		sprintf (lpsTexLightMode, "[F3] texture unlighted");

	sprintf (lpsTexFilterMode, "[F4] %s", alpszTexFilterModes [gTexFilter]);
	sprintf (lpsSrcBlendMode, "[F7] %s", alpszSrcAlphaBlendModes [gSrcAlpha]);
	sprintf (lpsDstBlendMode, "[F8] %s", alpszDstAlphaBlendModes [gDstAlpha]);

	// get DC of current background bitmap plain offscreen surface
	if (glpDDSOne->GetDC(&hdc) == DD_OK)
	{
		SetBkColor (hdc, RGB( 0, 0, 0));
		SetTextColor (hdc, RGB( 255, 255, 255));

		// get text metric information
		GetTextMetrics (hdc, &tm);

		// paint a black strip at the bottom of the bitmap
		PatBlt (hdc, 0, 480 - (2 * ((int)tm.tmAscent + (int)tm.tmInternalLeading)),	640,
			2 * ((int)tm.tmAscent + (int)tm.tmInternalLeading), BLACKNESS);

		// write the instruction string
		TextOut (hdc, 0, 480 - (2*((int)tm.tmAscent + (int)tm.tmInternalLeading)),
			lpsTextureMode, lstrlen(lpsTextureMode));

		TextOut (hdc, 213, 480 - (2*((int)tm.tmAscent + (int)tm.tmInternalLeading)),
			lpsShadeMode, lstrlen(lpsShadeMode));

		TextOut (hdc, 426, 480 - (2*((int)tm.tmAscent + (int)tm.tmInternalLeading)),
			lpsTexLightMode, lstrlen(lpsTexLightMode));

		TextOut (hdc, 0, 480 - (int)tm.tmAscent - (int)tm.tmInternalLeading,
			lpsTexFilterMode, lstrlen(lpsTexFilterMode));

		TextOut (hdc, 213, 480 - (int)tm.tmAscent - (int)tm.tmInternalLeading,
			lpsSrcBlendMode, lstrlen(lpsSrcBlendMode));

		TextOut (hdc, 426, 480 - (int)tm.tmAscent - (int)tm.tmInternalLeading,
			lpsDstBlendMode, lstrlen(lpsDstBlendMode));

		glpDDSOne->ReleaseDC (hdc);
	}
}
