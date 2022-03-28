#include <windows.h>
#include <windowsx.h>
//#include "c:\msdev\include\mmsystem.h"
//#include <vfw.h>
//#include <mciavi.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "winh.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>

#include "ati3dcif.h"


char *inp[]={
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\menu\\menus.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\altima.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\karbonis\\karbonis.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\terramax\\terramax.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\korodera\\korodera.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\arridos\\arridos.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\silver\\silver.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\firestar\\firestar.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\altima\\altima.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\karbonis\\karbonis.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\terramax\\terramax.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\korodera\\korodera.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\arridos\\arridos.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\silver\\silver.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\firestar\\firestar.bmp"
};

char *out[]={
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\menu\\menus.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\altima.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\karbonis\\karbonis.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\terramax\\terramax.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\korodera\\korodera.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\arridos\\arridos.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\silver\\silver.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\firestar\\firestar.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\altima\\altima.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\karbonis\\karbonis.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\terramax\\terramax.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\korodera\\korodera.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\arridos\\arridos.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\silver\\silver.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\rapier\\firestar\\firestar.sjr"
};

/*
 * WinMain - initialization, message loop
 */
int PASCAL      WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow)
{
    HRESULT             ddretval;
	C3D_TMAP			TMap;
	int					k, maxlog2,i,ii;
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
	int	x,y,count,whichSurface;
	HANDLE           hFileMapping;
	LPBYTE           pBase;
	int fp;
	C3D_UINT16		*memst, *ptr;
	int memsz;

	memst = (C3D_UINT16 *)malloc(6000000);

	for(ii=0;ii<1;++ii)
	{
	ptr = memst;


	fp = open(
	out[ii],
	O_RDWR|O_CREAT|O_BINARY);
	// open texture map file for reading
	hTexFile = CreateFile (
	inp[ii],
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_RANDOM_ACCESS,
		NULL);

	// read headers and bitmap data
	// first, determine buffer size required for BITMAPINFO and data
	bufsize = GetFileSize (hTexFile, NULL);

        hFileMapping = CreateFileMapping( hTexFile, NULL, PAGE_READONLY, 0, 0, NULL );
        if( hFileMapping == NULL ) {
            return FALSE;
        }

        pBase = MapViewOfFile( hFileMapping, FILE_MAP_READ, 0, 0, 0 );
        if( pBase == NULL ) {
             return FALSE;
        }
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
	memsz = 0;
	for (y=8192-1,count=0; y>=0; y--,count++)
	{
		if(count<1280 || ((count>=16*256) && (count < 21*256)))
			continue;
		if(count >= 1280+1024 && (count & 1))
			continue;
		// scan line pixel counter
		for (x=0; x<256; x++)
		{
			if(count >= 1280+1024 && (x & 1))
				continue;
			
			// read RGB
			memcpy (&b, (char*)bmdata + (y*pitch)+(3*x), sizeof (C3D_UINT8));
			memcpy (&g, (char*)bmdata + (y*pitch)+(3*x)+1, sizeof (C3D_UINT8));
			memcpy (&r, (char*)bmdata + (y*pitch)+(3*x)+2, sizeof (C3D_UINT8));

			rgb565 = ((C3D_UINT32)r & 0xf8) << 7 |
					 ((C3D_UINT32)g & 0xf8) << 2 |
					 ((C3D_UINT32)b & 0xf8) >> 3;
			*(ptr++) = rgb565;
			memsz++;
			// write texture data to texture cache
		}
	}

    if( hFileMapping != NULL && pBase ) {
        UnmapViewOfFile( pBase );
    }
    if( hFileMapping != NULL ) {
        CloseHandle( hFileMapping );
        hFileMapping = NULL;
    }
	// close the texture file; we're done with it
	CloseHandle (hTexFile);
	memsz = memsz * sizeof(C3D_UINT16);
	write(fp, (void *)memst,memsz);
	close(fp);
	}
	return 0;

}


