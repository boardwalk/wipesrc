/*==========================================================================
 *
 *  Copyright (C) 1995 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddex1.cpp
 *  Content:    Direct Draw example program 1.  Creates a Direct Draw 
 *              object and then a primary surface with a back buffer.
 *              Slowly flips between the primary surface and the back
 *              buffer.  Press F12 to terminate the program.
 *
 ***************************************************************************/

#define NAME "DDExample1"
#define TITLE "Direct Draw Example 1"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"

#define TIMER_ID        1
#define TIMER_RATE      500

LPDIRECTDRAW            lpDD;           // DirectDraw object
LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
BOOL                    bActive;        // is application active?

/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void finiObjects( void )
{
    if( lpDD != NULL )
    {
        if( lpDDSPrimary != NULL )
        {
            lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
            lpDDSPrimary = NULL;
        }
        lpDD->lpVtbl->Release(lpDD);
        lpDD = NULL;
    }
} /* finiObjects */

char szMsg[] = "Page Flipping Test: Press F12 to exit";
char szFrontMsg[] = "Front buffer (F12 to quit)";
char szBackMsg[] = "Back buffer (F12 to quit)";

long FAR PASCAL WindowProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    RECT        rc;
    SIZE        size;
    static BYTE phase = 0;
    HDC         hdc;

    switch( message )
    {
    case WM_ACTIVATEAPP:
        bActive = wParam;
        break;

    case WM_CREATE:
        break;

    case WM_SETCURSOR:
        SetCursor(NULL);
        return TRUE;

    case WM_TIMER:
    {
            DDSURFACEDESC       ddsd;
            LPBYTE              ptr;
	    int			i;
	    HRESULT 		ddrval;

        // Flip surfaces
        if( bActive )
        {
/**/
            if (lpDDSBack->lpVtbl->GetDC(lpDDSBack, &hdc) == DD_OK)
            {
                SetBkColor( hdc, RGB( 0, 0, 255 ) );
                SetTextColor( hdc, RGB( 255, 255, 0 ) );
                if( phase )
                {
                    TextOut( hdc, 0, 0, szFrontMsg, lstrlen(szBackMsg) );
                    phase = 0;
                }
                else
                {
                    TextOut( hdc, 0, 0, szBackMsg, lstrlen(szBackMsg) );
                    phase = 1;
                }
                lpDDSBack->lpVtbl->ReleaseDC(lpDDSBack,hdc);
            }
/**/

            ZeroMemory(&ddsd, sizeof(ddsd));
            ddsd.dwSize = sizeof(ddsd);

            ddrval = lpDDSBack->lpVtbl->Lock(lpDDSBack, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

            if (ddrval == DDERR_SURFACELOST)
	    {
		MessageBeep(0);
                lpDDSBack->lpVtbl->Restore(lpDDSBack);
	    }

	    ptr = (LPBYTE)ddsd.lpSurface;

	    for (i = 0; i < 320; i++)
	    {
		*(ptr+(2*ddsd.lPitch)) = i;
		*ptr++ = i;
	    }

            lpDDSBack->lpVtbl->Unlock(lpDDSBack, ddsd.lpSurface);

            while( 1 )
            {
                HRESULT ddrval;
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


        

        break;
    }

 
    case WM_KEYDOWN:
        switch( wParam )
        {
        case VK_ESCAPE:
        case VK_F12:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    case WM_PAINT:
        BeginPaint( hWnd, &ps );
        GetClientRect(hWnd, &rc);
        GetTextExtentPoint( ps.hdc, szMsg, lstrlen(szMsg), &size );
        SetBkColor( ps.hdc, RGB( 0, 0, 255 ) );
        SetTextColor( ps.hdc, RGB( 255, 255, 0 ) );
        TextOut( ps.hdc, (rc.right - size.cx)/2, (rc.bottom - size.cy)/2,
            szMsg, sizeof( szMsg )-1 );
        EndPaint( hWnd, &ps );
        break;

    case WM_DESTROY:
        finiObjects();
        PostQuitMessage( 0 );
        break;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);

} /* WindowProc */

/*
 * doInit - do work required for every instance of the application:
 *                create the window, initialize data
 */
static BOOL doInit( HANDLE hInstance, int nCmdShow )
{
    HWND                hwnd;
    WNDCLASS            wc;
    DDSURFACEDESC       ddsd;
    DDSCAPS             ddscaps;
    HRESULT             ddrval;
    HDC                 hdc;
    char                buf[256];

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
    hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        NAME,
        TITLE,
        WS_POPUP,
        0, 0,
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
     * create the main DirectDraw object
     */
    ddrval = DirectDrawCreate( NULL, &lpDD, NULL );
    if( ddrval == DD_OK )
    {
        // Get exclusive mode
        ddrval = lpDD->lpVtbl->SetCooperativeLevel( lpDD, hwnd,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX );
        if(ddrval == DD_OK )
        {
            ddrval = lpDD->lpVtbl->SetDisplayMode( lpDD, 320, 200, 8 );
            if( ddrval == DD_OK )
            {
                // Create the primary surface with 1 back buffer
                ddsd.dwSize = sizeof( ddsd );
                ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
                ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                                      DDSCAPS_FLIP | 
                                      DDSCAPS_COMPLEX;
                ddsd.dwBackBufferCount = 1;
                ddrval = lpDD->lpVtbl->CreateSurface( lpDD, &ddsd, &lpDDSPrimary, NULL );
                if( ddrval == DD_OK )
                {
                    // Get a pointer to the back buffer
                    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
                    ddrval = lpDDSPrimary->lpVtbl->GetAttachedSurface(lpDDSPrimary,
                                                        &ddscaps,
                                                          &lpDDSBack);
                    if( ddrval == DD_OK )
                    {
                        // draw some text.
                        if (lpDDSPrimary->lpVtbl->GetDC(lpDDSPrimary, &hdc) == DD_OK)
                        {
                            SetBkColor( hdc, RGB( 0, 0, 255 ) );
                            SetTextColor( hdc, RGB( 255, 255, 0 ) );
                            TextOut( hdc, 0, 0, szFrontMsg, lstrlen(szFrontMsg) );
                            lpDDSPrimary->lpVtbl->ReleaseDC(lpDDSPrimary, hdc);
                        }

                        if (lpDDSBack->lpVtbl->GetDC(lpDDSBack, &hdc) == DD_OK)
                        {
                            SetBkColor( hdc, RGB( 0, 0, 255 ) );
                            SetTextColor( hdc, RGB( 255, 255, 0 ) );
                            TextOut( hdc, 0, 0, szBackMsg, lstrlen(szBackMsg) );
                            lpDDSBack->lpVtbl->ReleaseDC(lpDDSBack, hdc);
                        }

                        // Create a timer to flip the pages
                        if( SetTimer( hwnd, TIMER_ID, TIMER_RATE, NULL ) )
                        {
                             return TRUE;
                        }
                    }
                }
            }
        }
    }

    wsprintf(buf, "Direct Draw Init Failed (%08lx)\n", ddrval );
    MessageBox( hwnd, buf, "ERROR", MB_OK );
    finiObjects();
    DestroyWindow( hwnd );
    return FALSE;
} /* doInit */

#if 0
/*
 * WinMain - initialization, message loop
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                        LPSTR lpCmdLine, int nCmdShow)
{
    MSG         msg;

    lpCmdLine = lpCmdLine;
    hPrevInstance = hPrevInstance;

    if( !doInit( hInstance, nCmdShow ) )
    {
        return FALSE;
    }

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;

} /* WinMain */
#endif
