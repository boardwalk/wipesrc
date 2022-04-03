#define STRICT
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include "resource.h"
#include "textures.h"
#include "rGraphics.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
static LPDIRECTDRAW 		g_pDD1			 = NULL;
LPDIRECTDRAW4				g_pDD4			 = NULL;
LPDIRECTDRAWSURFACE4		g_pddsPrimary	 = NULL;
LPDIRECTDRAWSURFACE4		g_pddsBackBuffer = NULL;
static LPDIRECTDRAWSURFACE4 g_pddsZBuffer	 = NULL;
static LPDIRECT3D3			g_pD3D			 = NULL;
LPDIRECT3DDEVICE3			g_pd3dDevice	 = NULL;
static LPDIRECT3DVIEWPORT3	g_pvViewport	 = NULL;
static RECT 				g_rcScreenRect;
static RECT 				g_rcViewportRect;
static HWND 				g_hWnd;
//-----------------------------------------------------------------------------
// Local variables for the Windows portion of the app
//-----------------------------------------------------------------------------
BOOL				g_bAppUseFullScreen = FALSE;
BOOL				g_bSoftwareOnly	   = FALSE;
BOOL				CurrentAlphaTest=FALSE;
BOOL				CurrentAlphaBlend=FALSE;
BOOL				CurrentTClamp[2]={FALSE,FALSE};
BOOL				CurrentSClamp[2]={FALSE,FALSE};
BOOL				Screen565=FALSE;
LPDIRECT3DTEXTURE2	CurrentTexture[2];
D3DTLVERTEX vert[3];
D3DCOLOR CurrentColor;
BOOL TwoTextures = FALSE;
int32_t TextureStages = 0;
#define TEXTURES_AVAILABLE 200
//-----------------------------------------------------------------------------
// Name: EnumZBufferCallback()
// Desc: Enumeration function to report valid pixel formats for z-buffers.
//-----------------------------------------------------------------------------
static HRESULT WINAPI EnumZBufferCallback( DDPIXELFORMAT* pddpf,
										   VOID* pddpfDesired )
{
	if( pddpf->dwFlags == DDPF_ZBUFFER )
	{
		memcpy( pddpfDesired, pddpf, sizeof(DDPIXELFORMAT) );

		// Return with D3DENUMRET_CANCEL to end the search.
		return D3DENUMRET_CANCEL;
	}

	// Return with D3DENUMRET_OK to continue the search.
	return D3DENUMRET_OK;
}

//-----------------------------------------------------------------------------
// Name: Initialize3DEnvironment()
// Desc: This function initializes all the DirectDraw/Direct3D objects used for
//		 3D-rendering.
//-----------------------------------------------------------------------------
HRESULT Initialize3DEnvironment( HWND hWnd, GUID* pDriverGUID,
								 const GUID* pDeviceGUID )
{
	HRESULT hr;
	DDSURFACEDESC2 ddsd;

	g_hWnd = hWnd;
	// Create the IDirectDraw interface. The first parameter is the GUID,
	// which is allowed to be NULL. If there are more than one DirectDraw
	// drivers on the system, a NULL guid requests the primary driver. For
	// non-GDI hardware cards like the 3DFX and PowerVR, the guid would need
	// to be explicity specified . (Note: these guids are normally obtained
	// from enumeration, which is convered in a subsequent tutorial.)
	hr = DirectDrawCreate( pDriverGUID, &g_pDD1, NULL );
	if( FAILED( hr ) )
		return hr;

	// Get a ptr to an IDirectDraw4 interface. This interface to DirectDraw
	// represents the DX6 version of the API.
	hr = g_pDD1->QueryInterface( IID_IDirectDraw4, (VOID**)&g_pDD4 );
	if( FAILED( hr ) )
		return hr;

	// Set the Windows cooperative level
	DWORD dwCoopFlags = DDSCL_NORMAL;
	if( g_bAppUseFullScreen )
		dwCoopFlags = DDSCL_ALLOWREBOOT|DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN;

	// By default, set the flag to allow D3D to optimize floating point calcs
	dwCoopFlags |= DDSCL_FPUSETUP;

	hr = g_pDD4->SetCooperativeLevel( hWnd, dwCoopFlags );
	if( FAILED( hr ) )
		return hr;

	if( g_bAppUseFullScreen )
	{
		// Get the dimensions of the viewport and screen bounds
		// Store the rectangle which contains the renderer
		SetRect( &g_rcViewportRect, 0, 0, 640, 480 );
		memcpy( &g_rcScreenRect, &g_rcViewportRect, sizeof(RECT) );

		// Set the display mode to the requested dimensions. Check for
		// 320x200x8 modes, and set flag to avoid using ModeX
		DWORD dwModeFlags = 0;
#if 0
		if( (320==m_dwRenderWidth) && (200==m_dwRenderHeight) &&
			(8==pddsd->ddpfPixelFormat.dwRGBBitCount) )
			dwModeFlags |= DDSDM_STANDARDVGAMODE;
#endif
		if( FAILED( hr = g_pDD4->SetDisplayMode( 640, 480, 16, 0, dwModeFlags ) ) )
			return hr;

		// Create the primary surface

		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize 				= sizeof(DDSURFACEDESC2);
		ddsd.dwFlags				= DDSD_CAPS;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_3DDEVICE;

		ddsd.dwFlags		  |= DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps   |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;

		if( FAILED( hr = g_pDD4->CreateSurface( &ddsd, &g_pddsPrimary, NULL ) ) )
			return hr;

		g_pddsPrimary->GetSurfaceDesc(&ddsd);
		if(ddsd.ddpfPixelFormat.dwGBitMask & 0x20)
			Screen565 = TRUE;
		// Get a ptr to the back buffer, which will be our render target
		DDSCAPS2 ddscaps;
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		if( FAILED( hr = g_pddsPrimary->GetAttachedSurface( &ddscaps, &g_pddsBackBuffer ) ) )
			return hr;
	}
	else // Set up buffers for windowed rendering
	{
		ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
		ddsd.dwSize 		= sizeof(DDSURFACEDESC2);
		ddsd.dwFlags		= DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		// Create the primary surface.
		hr = g_pDD4->CreateSurface( &ddsd, &g_pddsPrimary, NULL );
		if( FAILED( hr ) )
			return hr;

		g_pddsPrimary->GetSurfaceDesc(&ddsd);
		if(ddsd.ddpfPixelFormat.dwGBitMask & 0x20)
			Screen565 = TRUE;
		// Setup a surface description to create a backbuffer. This is an
		// offscreen plain surface with dimensions equal to our window size.
		// The DDSCAPS_3DDEVICE is needed so we can later query this surface
		// for an IDirect3DDevice interface.
		ddsd.dwFlags		= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
		if(g_bSoftwareOnly)
			ddsd.ddsCaps.dwCaps|= DDSCAPS_SYSTEMMEMORY;

		// Set the dimensions of the backbuffer. Note that if our window changes
		// size, we need to destroy this surface and create a new one.
		GetClientRect( hWnd, &g_rcScreenRect );
		GetClientRect( hWnd, &g_rcViewportRect );
		ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.left );
		ClientToScreen( hWnd, (POINT*)&g_rcScreenRect.right );
		ddsd.dwWidth  = g_rcScreenRect.right - g_rcScreenRect.left;
		ddsd.dwHeight = g_rcScreenRect.bottom - g_rcScreenRect.top;

		// Create the backbuffer. The most likely reason for failure is running
		// out of video memory. (A more sophisticated app should handle this.)
		hr = g_pDD4->CreateSurface( &ddsd, &g_pddsBackBuffer, NULL );
		if( FAILED( hr ) )
			return hr;

		// Create a clipper object which handles all our clipping for cases when
		// our window is partially obscured by other windows. This is not needed
		// for apps running in fullscreen mode.
		LPDIRECTDRAWCLIPPER pcClipper;
		hr = g_pDD4->CreateClipper( 0, &pcClipper, NULL );
		if( FAILED( hr ) )
			return hr;

		// Associate the clipper with our window. Note that, afterwards, the
		// clipper is internally referenced by the primary surface, so it is safe
		// to release our local reference to it.
		pcClipper->SetHWnd( 0, hWnd );
		g_pddsPrimary->SetClipper( pcClipper );
		pcClipper->Release();
	}
	// Query DirectDraw for access to Direct3D
	   g_pDD4->QueryInterface( IID_IDirect3D3, (VOID**)&g_pD3D );
	   if( FAILED( hr) )
		return hr;

	//-------------------------------------------------------------------------
	// Create the z-buffer AFTER creating the backbuffer and BEFORE creating
	// the d3ddevice.
	//
	// Note: before creating the z-buffer, apps may want to check the device
	// caps for the D3DPRASTERCAPS_ZBUFFERLESSHSR flag. This flag is true for
	// certain hardware that can do HSR (hidden-surface-removal) without a
	// z-buffer. For those devices, there is no need to create a z-buffer.
	//-------------------------------------------------------------------------

	DDPIXELFORMAT ddpfZBuffer;
	g_pD3D->EnumZBufferFormats( *pDeviceGUID,
								EnumZBufferCallback, (VOID*)&ddpfZBuffer );

	// If we found a good zbuffer format, then the dwSize field will be
	// properly set during enumeration. Else, we have a problem and will exit.
	if( sizeof(DDPIXELFORMAT) != ddpfZBuffer.dwSize )
		return E_FAIL;

	// Get z-buffer dimensions from the render target
	// Setup the surface desc for the z-buffer.
	ddsd.dwFlags		= DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_ZBUFFER;
	ddsd.dwWidth		= g_rcScreenRect.right - g_rcScreenRect.left;
	ddsd.dwHeight		= g_rcScreenRect.bottom - g_rcScreenRect.top;
	memcpy( &ddsd.ddpfPixelFormat, &ddpfZBuffer, sizeof(DDPIXELFORMAT) );

	// For hardware devices, the z-buffer should be in video memory. For
	// software devices, create the z-buffer in system memory
	if( IsEqualIID( *pDeviceGUID, IID_IDirect3DHALDevice ) )
		ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;

	// Create and attach a z-buffer. Real apps should be able to handle an
	// error here (DDERR_OUTOFVIDEOMEMORY may be encountered). For this
	// tutorial, though, we are simply going to exit ungracefully.
	if( FAILED( hr = g_pDD4->CreateSurface( &ddsd, &g_pddsZBuffer, NULL ) ) )
		return hr;

	// Attach the z-buffer to the back buffer.
	if( FAILED( hr = g_pddsBackBuffer->AddAttachedSurface( g_pddsZBuffer ) ) )
		return hr;

	//-------------------------------------------------------------------------
	// End of z-buffer creation code.
	//
	// Before rendering, don't forget to enable the z-buffer with the
	// appropiate D3DRENDERSTATE's.
	//-------------------------------------------------------------------------

	// Before creating the device, check that we are NOT in a palettized
	// display. That case will cause CreateDevice() to fail, since this simple
	// tutorial does not bother with palettes.
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	g_pDD4->GetDisplayMode( &ddsd );
	if( ddsd.ddpfPixelFormat.dwRGBBitCount <= 8 )
		return DDERR_INVALIDMODE;

	// Create the device. The device is created off of our back buffer, which
	// becomes the render target for the newly created device. Note that the
	// z-buffer must be created BEFORE the device

	if( FAILED( hr = g_pD3D->CreateDevice( *pDeviceGUID, g_pddsBackBuffer,
										   &g_pd3dDevice, NULL ) ) )
	{
		// This call could fail for many reasons. The most likely cause is
		// that we specifically requested a hardware device, without knowing
		// whether there is even a 3D card installed in the system. Another
		// possibility is the hardware is incompatible with the current display
		// mode (the correct implementation would use enumeration for this.)
		return hr;
	}

	// Set up the viewport data parameters
	D3DVIEWPORT2 vdData;
	ZeroMemory( &vdData, sizeof(D3DVIEWPORT2) );
	vdData.dwSize		= sizeof(D3DVIEWPORT2);
	vdData.dwWidth		= g_rcScreenRect.right - g_rcScreenRect.left;
	vdData.dwHeight 	= g_rcScreenRect.bottom - g_rcScreenRect.top;
	vdData.dvClipX		= 0.0f;
	vdData.dvClipWidth	= vdData.dwWidth;
	vdData.dvClipY		= 0.0f;
	vdData.dvClipHeight = vdData.dwHeight;
	vdData.dvMaxZ		= 1.0f;

	// Create the viewport
	hr = g_pD3D->CreateViewport( &g_pvViewport, NULL );
	if( FAILED( hr ) )
		return hr;

	// Associate the viewport with the D3DDEVICE object
	g_pd3dDevice->AddViewport( g_pvViewport );

	// Set the parameters to the new viewport
	g_pvViewport->SetViewport2( &vdData );

	// Set the viewport as current for the device
	g_pd3dDevice->SetCurrentViewport( g_pvViewport );

	EnumTextureFormats();

	D3DDEVICEDESC hwCaps,swCaps;
	hwCaps.dwSize = swCaps.dwSize = sizeof(D3DDEVICEDESC);

	g_pd3dDevice->GetCaps(&hwCaps,&swCaps);

	if(g_bSoftwareOnly)
		TextureStages = swCaps.wMaxTextureBlendStages;
	else
	{
		if(!TextureStages)		// for Voodoo2 bug
			TextureStages = hwCaps.wMaxTextureBlendStages;
	}


	g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE, FALSE );
	g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER );
	g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	g_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE, TRUE );
//	g_pd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME );
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);
	// Finish by setting up our scene
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup3DEnvironment()
// Desc: Releases all the resources used by the app. Note the check for
//		 reference counts when releasing the D3DDevice and DDraw objects. If
//		 these ref counts are non-zero, then something was not cleaned up
//		 correctly.
//-----------------------------------------------------------------------------
HRESULT Cleanup3DEnvironment()
{
	// Release the DDraw and D3D objects used by the app
	if( g_pvViewport )
		g_pvViewport->Release();

	// Do a safe check for releasing the D3DDEVICE. RefCount should be zero.
	if( g_pd3dDevice )
		if( 0 < g_pd3dDevice->Release() )
			return E_FAIL;

	if( g_pddsZBuffer )
		g_pddsZBuffer->Release();
	if(!g_bAppUseFullScreen)
		if( g_pddsBackBuffer )
			g_pddsBackBuffer->Release();
	if( g_pddsPrimary )
		g_pddsPrimary->Release();
	if( g_pD3D )
		g_pD3D->Release();
	if( g_pDD4 )
	{
		g_pDD4->SetCooperativeLevel( g_hWnd, DDSCL_NORMAL );
		g_pDD4->Release();
	}


	// Do a safe check for releasing DDRAW. RefCount should be zero.
	if( g_pDD1 )
		if( 0 < g_pDD1->Release() )
			return E_FAIL;

	g_pvViewport	 = NULL;
	g_pddsZBuffer	 = NULL;
	g_pd3dDevice	 = NULL;
	g_pD3D			 = NULL;
	g_pddsBackBuffer = NULL;
	g_pddsPrimary	 = NULL;
	g_pDD4			 = NULL;
	g_pDD1			 = NULL;

	return S_OK;
}

BOOL WINAPI Enumerate2ndCard(GUID FAR *lpGUID,LPSTR desc, LPSTR name,LPVOID context,HMONITOR hm)
{
	GUID *myGUID = (GUID *)context;

	if(lpGUID == NULL)
		return TRUE;
	*myGUID = *lpGUID;
	return FALSE;
}

BOOL renderInitialise(HWND hWnd, BOOL b_fullScreen, BOOL Voodoo, BOOL softwareOnly)
{
	GUID myGuid;
	GUID *pGuid=NULL;

	g_bSoftwareOnly = softwareOnly;
	g_bAppUseFullScreen = b_fullScreen;

	if(Voodoo)
	{
		g_bSoftwareOnly = FALSE;
		g_bAppUseFullScreen = TRUE;
		DirectDrawEnumerateEx(Enumerate2ndCard,&myGuid,DDENUM_NONDISPLAYDEVICES);
		pGuid = &myGuid;
		TextureStages = 1;		// for current Driver bug
	}
	// Cleanup any objects that might've been created before
	if( FAILED( Cleanup3DEnvironment() ) )
		return E_FAIL;

	if(!softwareOnly)
	{
		// Create the D3D environment, at first, trying the HAL
		if( SUCCEEDED( Initialize3DEnvironment( hWnd, pGuid, &IID_IDirect3DHALDevice ) ) )
			return S_OK;

		// Else, cleanup objects potentially created during the failed
		// initialization attempt.
		Cleanup3DEnvironment();
	}
	if( SUCCEEDED( Initialize3DEnvironment( hWnd, NULL, &IID_IDirect3DRGBDevice ) ) )
		return S_OK;

	// Else, return failure
	return E_FAIL;
}

void renderFlipSurface()
{
	if(g_bAppUseFullScreen)
		g_pddsPrimary->Flip( NULL,	DDFLIP_WAIT  | DDFLIP_NOVSYNC );
	else
		g_pddsPrimary->Blt( &g_rcScreenRect, g_pddsBackBuffer,
							   &g_rcViewportRect, DDBLT_WAIT, NULL );
}
