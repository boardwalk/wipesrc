/****************************************************************************
 *
 * Functions for:
 *
 * Initialises 3D subsystem and loads the RidgeRacer texture
 *
 ****************************************************************************
 *
 * $Log:   P:/ridger/ridger/stevew/init3d.c_v  $
 * 
 *    Rev 1.12   04 Sep 1995 13:58:48   AndyS
 * removed wipeout specifics from this file and put them in ajs.c
 * 
 *    Rev 1.11   29 Aug 1995 14:03:22   AndyS
 * multi palette stuff added
 * 
 *    Rev 1.10   25 Aug 1995 11:32:14   AndyY	  
 * Removed c:
 * 
 *    Rev 1.9   25 Aug 1995 10:50:22   AndyY
 * stripped c:
 * 
 *    Rev 1.8   23 Aug 1995 08:04:24   AndyY
 * 
 * added a CVECTOR Pallette and expanded the load for the Depth Thingys to
 * 96
 * 
 *    Rev 1.7   23 Aug 1995 00:23:18   AndyS
 * new load vram function for wipeout - also loads dpq and spd files
 * 
 *    Rev 1.6   28 Jul 1995 14:31:38   DaveS
 * A bit "streamlined" now
 * 
 *    Rev 1.5   27 Jul 1995 11:20:34   DaveS
 * Fixed palette problem
 * 
 *    Rev 1.4   27 Jul 1995 10:41:42   AndyS
 * This had better work Andy!
 * 
 *    Rev 1.3   18 Jul 1995 13:15:50   DaveS
 * Changed the PCX reader
 * 
 *    Rev 1.2   18 Jul 1995 10:57:02   AndyY
 * New version of Steve's init3d whiuch sorts out depth fading
 *
 *	
 ****************************************************************************/

#include "w:\h\stdio.h"
extern	void	InitFpTbl(void);

void init3d(void)
{
	FILE *infile;

	InitFpTbl();
}


