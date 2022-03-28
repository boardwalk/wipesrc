#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ddraw.h>
#include <d3d.h>

#include "textures.h"

extern LPDIRECT3DDEVICE3	g_pd3dDevice;
extern LPDIRECTDRAW4		g_pDD4;


TextureFormats availableTextures;
static void DeterminePackingShift (unsigned int mask, unsigned int *truncate_shift, unsigned int *up_shift);
int hacktext = 0;
static HRESULT CALLBACK EnumPixelFormatsCallback (DDPIXELFORMAT * pddpf, LPVOID lpContext)
{
	unsigned long m;
	int a, r, g, b;

	TextureFormats *tf = (TextureFormats *)lpContext;

	// Setup the new surface desc
	DDSURFACEDESC2 ddsd;

	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize                 = sizeof(DDSURFACEDESC2);
	ddsd.ddpfPixelFormat = *pddpf;
	ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
                           DDSD_PIXELFORMAT|DDSD_TEXTURESTAGE;
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE;
	ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	ddsd.dwTextureStage  = 0;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED4)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXEDTO8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_PALETTEINDEXED8)
		return DDENUMRET_OK;

	if(pddpf->dwFlags &  DDPF_RGB)
	{
		for (r = 0, m = pddpf->dwRBitMask; !(m & 1);
		     r++, m >>= 1);
		for (r = 0; m & 1; r++, m >>= 1);
		for (g = 0, m = pddpf->dwGBitMask; !(m & 1);
		     g++, m >>= 1);
		for (g = 0; m & 1; g++, m >>= 1);
			for (b = 0, m = pddpf->dwBBitMask; !(m & 1);
		     b++, m >>= 1);
		for (b = 0; m & 1; b++, m >>= 1);
	}
	a=0;
	if(pddpf->dwFlags &  DDPF_ALPHAPIXELS)
	{
		for (a = 0, m = pddpf->dwRGBAlphaBitMask; !(m & 1);
		     a++, m >>= 1);
		for (a = 0; m & 1; a++, m >>= 1);
	}
	if(a == 1 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface1555, &ddsd, sizeof(ddsd));
		tf->flags |= f1555;
	}
	if(a == 4 && r == 4 && g == 4 && b == 4)
	{
		memcpy(&tf->surface4444, &ddsd, sizeof(ddsd));
		tf->flags |= f4444;
	}
	if(a == 0 && r == 5 && g == 6 && b == 5)
	{
		memcpy(&tf->surface565, &ddsd, sizeof(ddsd));
		tf->flags |= f565;
	}
	if(a == 0 && r == 5 && g == 5 && b == 5)
	{
		memcpy(&tf->surface555, &ddsd, sizeof(ddsd));
		tf->flags |= f555;
	}
	return DDENUMRET_OK;
}

/*
 * D3DAppIEnumTextureFormats
 * Get a list of available texture map formats from the Direct3D driver by
 * enumeration.  Choose a default format (paletted is prefered).
 */
BOOL EnumTextureFormats (void)
{
	HRESULT hr;

	hr =	g_pd3dDevice->EnumTextureFormats (EnumPixelFormatsCallback, (LPVOID) & availableTextures);
	if (hr != DD_OK)
		return FALSE;
	return TRUE;
}
