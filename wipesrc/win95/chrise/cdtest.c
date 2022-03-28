/************************************************************************
*																								*
*									CD-ROM Interrupt test program						*
*																								*
*************************************************************************

/************************************************************************
*																								*
*			Les Includes																	*
*																								*
************************************************************************/

#include <stdio.h>
#include <dos.h>

extern union		REGS	regs;
extern struct		SREGS	sregs;

extern int drive;
extern unsigned char VTOCBuff[2048];

struct PlayDAHdr
 {
  
/************************************************************************
*																								*
*			El Function Prototypes														*
*																								*
************************************************************************/

int 	GetDriveInfo(void);
void 	GetVTOC(int drive, int index);
void	PlayDA(trackno);

/************************************************************************
*																								*
*			Da Code																			*
*																								*
************************************************************************/

main()
 {
  drive = GetDriveInfo;
  GetVTOC(drive,1);
 }

/************************************************************************
*																								*
*			Los Functions																	*
*																								*
************************************************************************/

int GetDriveInfo(void)
 {
  regs.w.ax = 0x1500;							/* Set up command code */
  regs.w.bx = 0;
  
  int386(0x2f, &regs, &regs);	  				/* Do int call to MSCDEX */

  if (regs.w.bx ==0)								/* Is MSCDEX installed ? */
   return(-1)
  else
   return regs.w.cx;								/* Return drive letter as no */
 }

/************************************************************************/

void GetVTOC(int drive, int index)
 {
	regs.w.ax = 0x1505;
	sregs.es = FP_SEG(VTOCBuff);				/* Set ES:BX to point	*/
	sregs.bx = FP_OFF(VTOCBuff);				/* to VTOCBuff */
	regs.w.cx = drive;
	regs.w.dx = index;

	int386x(0x2f, &regs, &regs, &sregs);	/* Do int call to MSCDEX */
 }

/************************************************************************/

void	PlayDA(trackno);
 {
  
