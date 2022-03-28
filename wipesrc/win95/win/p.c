/****************************************************************************
 *
 * win.c
 *
 ****************************************************************************
 *
 * $Log$
 * 
 ****************************************************************************/


/****************************************************************************
 *
 * Include Files
 *
 ****************************************************************************/

#define _WIN32
#include <res.h>
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>

#include <devices.h>
#include <scancode.h>


/****************************************************************************
 *
 * #defines and macros.
 *
 ****************************************************************************/

#define NAME "Assault Rigs"
#define TITLE "Assault Rigs"

/****************************************************************************
 *
 * User type definitions.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Structure definitions.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * Function prototypes.
 *
 ****************************************************************************/

/****************************************************************************
 *
 * External (global) variables.
 *
 ****************************************************************************/

LPDIRECTDRAW			lpDD;
LPDIRECTDRAWSURFACE		lpDDSPrimary;
LPDIRECTDRAWSURFACE		lpDDSBack;
LPDIRECTDRAWPALETTE		lpDDPalette;
BOOL				bActive;
PALETTEENTRY			ddPalette[256];

char szMsg[] = "Page Flipping Test: Press F12 to exit";
char szFrontMsg[] = "Front buffer (F12 to quit)";
char szBackMsg[] = "Back buffer (F12 to quit)";

HRESULT ddrval;


/****************************************************************************
 *
 * void TidyUpWin(void)
 *
 ****************************************************************************
 *
 * Tidy up objects once we're finished
 *
 ****************************************************************************/

void TidyUpWin(void)
{
	if(lpDD)
	{
		if(lpDDSPrimary)
		{
			lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
			lpDDSPrimary=NULL;
		}
		lpDD->lpVtbl->Release(lpDD);
		lpDD=NULL;
	}
}


/****************************************************************************
 *
 * long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
 *
 ****************************************************************************
 *
 * Window message handler
 *
 ****************************************************************************/

long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
//	PAINTSTRUCT ps;
//	RECT rc;
//	SIZE size;

	switch(message)
	{
		case WM_ACTIVATEAPP:
			bActive=wParam;
			break;
	
		case WM_CREATE:
			break;

		case WM_SETCURSOR:
			SetCursor(NULL);
			return(TRUE);

		case WM_KEYDOWN:
			{
				extern void FakeKeyboardHandler(int);
				FakeKeyboardHandler((lParam>>16) & 127);
			}
			break;

		case WM_KEYUP:
			{
				extern void FakeKeyboardHandler(int);
				FakeKeyboardHandler(((lParam>>16) & 127) | 128);
			}
			break;

		case WM_DESTROY:
			TidyUpWin();
			PostQuitMessage(0);
			break;

	}

	return(DefWindowProc(hWnd,message,wParam,lParam));
}

/****************************************************************************
 *
 * BOOL WinCopyScreen(unsigned char *drawScreenPtr)
 *
 ****************************************************************************
 *
 * Copy drawScreenPtr to the screen...
 *
 ****************************************************************************/

void WinCopyScreen(unsigned char *drawScreenPtr)
{
	LPBYTE ptr;
	DDSURFACEDESC	ddsd;
	BYTE buf[256];
	int i;

	GetKeyboardState(&buf[0]);
	if(buf[VK_RCONTROL]&128)
	{
		MessageBeep(0);
	}

//	if(GetAsyncKeyState(VK_MENU)>>7)
//	{
//		MessageBeep(0);
//	}

	/* flip surface */
	if(bActive)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		ddrval = lpDDSBack->lpVtbl->Lock(lpDDSBack, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

		if (ddrval == DDERR_SURFACELOST)
		{
			MessageBeep(0);
			lpDDSBack->lpVtbl->Restore(lpDDSBack);
		}

		ptr = (LPBYTE)ddsd.lpSurface;

		for(i=0;i<YRES;i++)
		{
			memcpy(ptr,drawScreenPtr+(i*XRES),XRES);
			ptr+=ddsd.lPitch;
		}

		lpDDSBack->lpVtbl->Unlock(lpDDSBack, ddsd.lpSurface);

		while( 1 )
		{
			ddrval = lpDDSPrimary->lpVtbl->Flip(lpDDSPrimary, NULL, 0 );
			if( ddrval == DD_OK )
			{
				break;
			}
			if( ddrval == DDERR_SURFACELOST )
			{
				ddrval = lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
				if( ddrval != DD_OK )
				{
					break;
				}
			}
			if( ddrval != DDERR_WASSTILLDRAWING )
			{
				break;
			}
		}
	}
}



/****************************************************************************
 *
 * BOOL InitWin(HANDLE hInstance, int nCmdShow)
 *
 ****************************************************************************
 *
 * Create window, initialise data...
 *
 ****************************************************************************/

BOOL InitWin(HANDLE hInstance, int nCmdShow)
{
	HWND hwnd;
	WNDCLASS wc;
	DDSURFACEDESC ddsd;
	DDSCAPS ddscaps;
	HRESULT ddrval;
	HDC hdc;
	char buf[256];

	int i;

	/*
	 * Initialise palette
	 */

	for(i=0;i<256;i++)
	{
		ddPalette[i].peRed=i;
		ddPalette[i].peGreen=i;
		ddPalette[i].peBlue=i;
		ddPalette[i].peFlags=0;
	}

	/*
	 * Register window class
	 */

	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=WindowProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=NULL;
	wc.hCursor=NULL;
	wc.hbrBackground=NULL;
	wc.lpszMenuName=NAME;
	wc.lpszClassName=NAME;

	RegisterClass(&wc);

	/*
	 * Create window
	 */

	hwnd=CreateWindowEx(
		WS_EX_TOPMOST,
		NAME,
		TITLE,
		WS_POPUP,
		0,0,
		GetSystemMetrics( SM_CXSCREEN ),
		GetSystemMetrics( SM_CYSCREEN ),
		NULL,
		NULL,
		hInstance,
		NULL );

	if(!hwnd)
	{
		return(FALSE);
	}

	ShowWindow(hwnd,nCmdShow);
	UpdateWindow(hwnd);

	/*
	 * Create DirectDraw object
	 */

	ddrval = DirectDrawCreate( NULL, &lpDD, NULL );

	if( ddrval == DD_OK )
	{
		/* get exclusive mode */

		ddrval = lpDD->lpVtbl->SetCooperativeLevel( lpDD, hwnd,
	                         DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX );

		if(ddrval == DD_OK )
		{
			ddrval = lpDD->lpVtbl->SetDisplayMode( lpDD, XRES, YRES, 8 );
			if( ddrval == DD_OK )
			{
				/* create a palette */
				ddrval = lpDD->lpVtbl->CreatePalette(lpDD,DDPCAPS_8BIT,ddPalette,&lpDDPalette,NULL);
				if(ddrval == DD_OK)
				{
					/* create the primary surface with 1 back buffer */
					ddsd.dwSize = sizeof( ddsd );
					ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
					ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
					                DDSCAPS_FLIP | 
					                DDSCAPS_COMPLEX;
					ddsd.dwBackBufferCount = 1;
					ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpDDSPrimary, NULL );
					if( ddrval == DD_OK )
					{
						/* set palette for this surface */
						ddrval=lpDDSPrimary->lpVtbl->SetPalette(lpDDSPrimary,lpDDPalette);

						/* Get a pointer to the back buffer */
						ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
						ddrval = lpDDSPrimary->lpVtbl->GetAttachedSurface(lpDDSPrimary,&ddscaps,&lpDDSBack);
						if( ddrval == DD_OK )
						{
							return(TRUE);

//							/* Create a timer to flip the pages */
//							if( SetTimer( hwnd, TIMER_ID, TIMER_RATE, NULL ) )
//							{
//								return TRUE;
//							}
						}
					}    
				}
			}
		}
	}

	wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
	MessageBox( hwnd, buf, "ERROR", MB_OK );
	TidyUpWin();
	DestroyWindow( hwnd );
	return FALSE;
}

/****************************************************************************
 *
 * void WinChangePalette(void)
 *
 ****************************************************************************
 *
 * Read the 3d-system's internal palette, and use it to update the direct
 * draw palette.
 *
 ****************************************************************************/

void WinChangePalette(void)
{
	HRESULT ddrval;
	int i;
	extern BYTE internalPaletteG[];

	for(i=0;i<256;i++)
	{
		ddPalette[i].peRed=internalPaletteG[i*3]*4;
		ddPalette[i].peGreen=internalPaletteG[i*3+1]*4;
		ddPalette[i].peBlue=internalPaletteG[i*3+2]*4;
		ddPalette[i].peFlags=0;
	}

	ddrval=lpDDPalette->lpVtbl->SetEntries(lpDDPalette,
	                                       DDSETPAL_IMMEDIATE,
						    0,
						    256,
						    ddPalette);
}

/****************************************************************************
 *
 * int ProcessMessages(void)
 *
 ****************************************************************************
 *
 * Process all messages in the message queue... does not block.
 * Called regularly from the main game loop.
 *
 ****************************************************************************/

int ProcessMessages(void)
{
	MSG msg;

	while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
	{
		if(!GetMessage(&msg,NULL,0,0))
		{
			extern int shutdownFlagG;
			shutdownFlagG=1;
			return(1);
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(0);
}

void KeyboardGrabSnapshotHook(void)
{
	ProcessMessages();
}

extern void OldMain(void);
extern void (*keyboardGrabSnapshotCallback)(void);

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	lpCmdLine;
	hPrevInstance;

	keyboardGrabSnapshotCallback=KeyboardGrabSnapshotHook;

	if(!InitWin(hInstance,nCmdShow))
	{
		return(FALSE);
	}

	OldMain();

	return(0);
}
