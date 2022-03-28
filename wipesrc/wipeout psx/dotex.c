#include <windows.h>
#include <windowsx.h>
#include <vfw.h>
#include <mciavi.h>
#include <stdlib.h>
#include <stdarg.h>
#include "..\ati3dcif.h"


typedef struct {
	LPDIRECTDRAWSURFACE	lpDDSTex;
	DDSURFACEDESC       ddsd;
	C3D_HTX				hTX;
} TEXTURE, * PTEXTURE;

TEXTURE gTex[22];

void main()
{
	const char* lpszTexFilename = "d:\wipesrc\wim95\wipeout\newgraph\menu\menus.bmp";
	int fp;
    HRESULT             ddretval;
	C3D_TMAP			TMap;
	int					k, maxlog2;
	C3D_EC				ecRetVal;
	HANDLE				hTexFile;
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmih;
	C3D_UINT32			bufsize;
	char*				bmdata;
	short*				p1;
	C3D_UINT32			pitch;
	C3D_UINT8			r, g, b;
	C3D_UINT16			rgb565;
	int	x,y,count,whichSurface,i;
	HANDLE           hFileMapping;
	LPBYTE           pBase;

	// open texture map file for reading
	hTexFile = CreateFile (
		lpszTexFilename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS,
		NULL);
	/*if (hTexFile == INVALID_HANDLE_VALUE)
	{
		wsprintf (gszErrMsg, "Could not open texture map file");
		return FALSE;
	}*/

	// read headers and bitmap data
	// first, determine buffer size required for BITMAPINFO and data
	bufsize = GetFileSize (hTexFile, NULL);
	/*if (bufsize == 0xFFFFFFFFL)
	{
		wsprintf (gszErrMsg, "Error getting texture file size");
		CloseHandle (hTexFile);
		return FALSE;
	}*/

        hFileMapping = CreateFileMapping( hTexFile, NULL, PAGE_READONLY, 0, 0, NULL );
        //if( hFileMapping == NULL ) {
            //return FALSE;
        //}

        pBase = MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
        //if( pBase == NULL ) {
             //return FALSE;
        //}
	// adjust bufsize for BITMAFILEHEADER
	bufsize -= sizeof (bmfh);

	// read BITMAPFILEHEADER
	memcpy(&bmfh,pBase,sizeof(bmfh));
	pBase+=sizeof(bmfh);

	// read BITMAPINFO and bitmap data
	memcpy (&bmih, pBase, sizeof (BITMAPINFOHEADER));
	pBase+=sizeof (BITMAPINFOHEADER);

	// unlock surface

	bmdata = pBase;
	// pitch of each scan line
	if(bmih.biSizeImage)
	{
		pitch = bmih.biSizeImage / bmih.biHeight;
	}
	else
	{
		pitch = bmih.biWidth * 3;		// JJS for BMPs without biSizeImage
	}

	whichSurface=-1;
	// line counter
	for (y=8192-1,count=0; y>=0; y-=2,count+=2)
	{
		if(count<1280 || ((count>=16*256) && (count < 21*256)))
			continue;
		if(count%256 == 0)
		{
			if(whichSurface>=0)
			{
				ddretval = gTex[whichSurface].lpDDSTex->lpVtbl->Unlock (gTex[whichSurface].lpDDSTex,NULL);
				/*if (ddretval != DD_OK)
				{
					wsprintf (gszErrMsg, "Error unlocking texture surface for file %s", lpszTexFilename);
					gTex[whichSurface].lpDDSTex->lpVtbl->Release (gTex[whichSurface].lpDDSTex);
					gTex[whichSurface].lpDDSTex = NULL;
					return FALSE;
				}*/
			}
			++whichSurface;
			// get a pointer to the texture surface
			ZeroMemory (&(gTex[whichSurface].ddsd), sizeof (gTex[whichSurface].ddsd));
			gTex[whichSurface].ddsd.dwSize = sizeof (gTex[whichSurface].ddsd);
			// lock texture to fill ddsd member
			ddretval = gTex[whichSurface].lpDDSTex->lpVtbl->Lock (gTex[whichSurface].lpDDSTex,
				NULL,
				&(gTex[whichSurface].ddsd),
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL);
			if (ddretval == DDERR_SURFACELOST) gTex[whichSurface].lpDDSTex->lpVtbl->Restore (gTex[whichSurface].lpDDSTex);
			p1=(short*)gTex[whichSurface].ddsd.lpSurface;
		}
		// scan line pixel counter
		for (x=0; x<256; x+=2)
		{
			// read RGB
			memcpy (&b, (char*)bmdata + (y*pitch)+(3*x), sizeof (C3D_UINT8));
			memcpy (&g, (char*)bmdata + (y*pitch)+(3*x)+1, sizeof (C3D_UINT8));
			memcpy (&r, (char*)bmdata + (y*pitch)+(3*x)+2, sizeof (C3D_UINT8));

			// pack color data in 16bpp format

//#define RGB_555
#ifndef RGB_555
			// pack data in RGB 555 format
			rgb565 = ((C3D_UINT32)r & 0xf8) << 8 |
					 ((C3D_UINT32)g & 0xfc) << 3 |
					 ((C3D_UINT32)b & 0xf8) >> 3;
#else
			// pack data in RGB 565 format
			rgb565 = ((C3D_UINT32)r & 0xf8) << 7 |
					 ((C3D_UINT32)g & 0xf8) << 2 |
					 ((C3D_UINT32)b & 0xf8) >> 3;
#endif
			*p1++=rgb565;
			// write texture data to texture cache
		}
	}

	ddretval = gTex[whichSurface].lpDDSTex->lpVtbl->Unlock (gTex[whichSurface].lpDDSTex,NULL);
	/*if (ddretval != DD_OK)
	{
		wsprintf (gszErrMsg, "Error unlocking texture surface for file %s", lpszTexFilename);
		gTex[whichSurface].lpDDSTex->lpVtbl->Release (gTex[whichSurface].lpDDSTex);
		gTex[whichSurface].lpDDSTex = NULL;
		return FALSE;
	}*/
    if( hFileMapping != NULL && pBase ) {
        UnmapViewOfFile( pBase );
    }
    if( hFileMapping != NULL ) {
        CloseHandle( hFileMapping );
        hFileMapping = NULL;
    }
	// close the texture file; we're done with it
	CloseHandle (hTexFile);

		fp = open("menus.sjr",O_RDWR|O_CREAT);
	for(i=0;i<22;++i)
	{
		// fill a C3D_TMAP struct
		ZeroMemory (&TMap, sizeof (TMap));
		TMap.u32Size = sizeof (TMap);

		// determine the maximum log2 dimension
		maxlog2 = 7;
		for (k=0; k < maxlog2; k++)
			TMap.apvLevels[k] = gTex[i].ddsd.lpSurface;

		TMap.bMipMap = FALSE;
		TMap.u32MaxMapXSizeLg2 = 7;
		TMap.u32MaxMapYSizeLg2 = 7;
		TMap.eTexFormat = C3D_ETF_RGB565;
//JJS	TMap.bTexTranspEnable = FALSE;
		SET_CIF_COLOR (TMap.clrTexChromaKey, 0, 0, 0, 0);

		write(fp, TMap, sizeof(TMap));



		// register the texture
		/*ecRetVal = ATI3DCIF_TextureReg (&TMap, &(gTex[i].hTX));
		if (ecRetVal != C3D_EC_OK)
		{
			wsprintf (gszErrMsg, "Error registering texture for file %s", lpszTexFilename);
			gTex[i].lpDDSTex->lpVtbl->Release (gTex[i].lpDDSTex);
			gTex[i].lpDDSTex = NULL;
			return FALSE;
		}*/
	}

	fclose(fp);
}
