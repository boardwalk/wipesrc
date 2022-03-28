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

typedef struct col
{
	C3D_UINT16 r;
	C3D_UINT16 g;
	C3D_UINT16 b;
} col;

char *inp[]={
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ANASTLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ANASTWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIALLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIALWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIANLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIANWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\CHANGLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\CHANGWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\DEKKALOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\DEKKAWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\PAULLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\PAULWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOLARLOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOLARWIN.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOPHILOS.bmp",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOPHIWIN.bmp"
};

char *out[]={
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ANASTLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ANASTWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIALLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIALWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIANLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\ARIANWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\CHANGLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\CHANGWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\DEKKALOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\DEKKAWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\PAULLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\PAULWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOLARLOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOLARWIN.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOPHILOS.sjr",
	"d:\\wipesrc\\win95\\wipeout\\newgraph\\venom\\altima\\SOPHIWIN.sjr"
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
	int xval;

	memst = (C3D_UINT16 *)malloc(6000000);

	for(ii=0;ii<16;++ii)
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
	for (y=140-1,count=0; y>=0; y--,count++)
	{
		xval = 0;
		//ignore blank bits

		// scan line pixel counter
		for (x=0; x<128; x++)
		{
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
		}//end for

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


