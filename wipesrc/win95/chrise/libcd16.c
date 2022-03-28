/************************************************************************
*																		*
*			Les Includes												*
*																		*
************************************************************************/

#include <stdio.h>
#include <i86.h>
#include <dos.h>
#include "libcd.h"

/************************************************************************
*																		*
*			El Structs													*
*																		*
************************************************************************/

union	REGS	regs;
struct	SREGS	sregs;


/************************************************************************
*																		*
*			El Function Prototypes										*
*																		*
************************************************************************/
/* test of PVCS*/

/************************************************************************
*																		*
*			Das Variables												*
*																		*
************************************************************************/

struct SeekReqHdr	SeekTable;
struct DevDriverHdr PauseHdr,ResumeHdr; 
struct DiskInfoHdr DiskInfoTable;
struct IOCTLHdr	IOCTLI;
struct TnoInfo	TrackInfoTable;
struct PlayReqHdr	PlayTable;
struct AudStatHdr	StatusTable;





struct	DiskInfoHdr *DiskInfo;
struct  TnoInfo *TrackInfo;

struct	SeekReqHdr *SeekInfo;
struct	AudStatHdr *StatusInfo;

/************************************************************************
*																		*
*			Los Functions												*
*																		*
************************************************************************/

/************************************************************************
*																		*
* 								GetDriveInfo16							*	
*																		*
*  PARAMETERS: None														*
*																		*
*  DESCRIPTION: Gets CD drive letter									* 
*																		*
*  RETURNS:	An integer giving drive letter (A=0, B=1, C=2, D=3...)		*
*																		*
************************************************************************/

 short GetDriveInfo16(void)
  {
   memset(&regs,0,sizeof(regs));
   
   regs.w.ax=0x1500;
   regs.w.bx=0;
   int86(0x2f,&regs,&regs);
   return(regs.w.cx);
  }

/************************************************************************
*																		*
*						   GetAudioDiskInfo16							*
*																		*
*  PARAMETERS: int drive												*
*																		*
*  DESCRIPTION:	Gets number of first and last audio tracks, plus		*
*				red book address of leadout track						*
*																		*
*  RETURNS:	A pointer to structure giving Disk Info						*
*																		*
************************************************************************/

 struct DiskInfoHdr *GetAudioDiskInfo16(short drive)
  {
   memset(&sregs,0,sizeof(sregs));
   memset(&regs,0,sizeof(regs)) ;
 
   //Set Up Disk Info

   DiskInfo->Command_Code=10;

   //Set Up IOCTLI

   //Set Up Device Driver Header
   
   IOCTLI.DDH.DDH_Length=13;					// Maybe 13 + 7 ??? But then again maybe not!
   IOCTLI.DDH.Subunit_Code=0;					// Check This !!
   IOCTLI.DDH.Command_Code=3;					// Code for IOCTL Input 
   IOCTLI.DDH.Status=0;							// Set on exit by device driver 

   //Set Up Rest Of IOCTL Block

   IOCTLI.MediaDescrip=0;
   IOCTLI.TransAddrOff=FP_OFF(DiskInfo);
   IOCTLI.TransAddrSeg=FP_SEG(DiskInfo);					
   IOCTLI.Bytes2Trans=7;
   IOCTLI.StartSectNo=0;
   IOCTLI.ID=0;
 
   //Set Up Regs

   regs.w.ax=0x1510;							// Send Device Driver Request Interrupt No 
   regs.w.cx=drive;
   sregs.es=FP_SEG(&IOCTLI);
   regs.w.bx=FP_OFF(&IOCTLI);

   int86x(0x2f,&regs, &regs, &sregs);

   return(DiskInfo);
  }

/************************************************************************
*																		*
*								 GetAudioAddr16							*
*																		*
*  PARAMETERS: int track												*
*																		*
*  DESCRIPTION: gets start address for given DA track no				*
*																		*
*  RETURNS:start address												*
*																		*
************************************************************************/

void GetAudioAddr16(short track,short drive)     
 {

  unsigned long SectorAddress;

  memset(&sregs,0,sizeof(sregs));
  memset(&regs,0,sizeof(regs)) ;

  //Set Up Disk Info

  TrackInfo->Command_Code=11;
  TrackInfo->Track=track;

  //Set Up IOCTLI

  //Set Up Device Driver Header
  
  IOCTLI.DDH.DDH_Length=13;						// Maybe 13 + 7 ??? But then again maybe not!
  IOCTLI.DDH.Subunit_Code=0;					// Check This !!
  IOCTLI.DDH.Command_Code=3;					// Code for IOCTL Input 
  IOCTLI.DDH.Status=0;							// Set on exit by device driver 

  //Set Up Rest Of IOCTL Block

  IOCTLI.MediaDescrip=0;
  IOCTLI.TransAddrOff=FP_OFF(TrackInfo);
  IOCTLI.TransAddrSeg=FP_SEG(TrackInfo);					
  IOCTLI.Bytes2Trans=7;
  IOCTLI.StartSectNo=0;
  IOCTLI.ID=0;

  //Set Up Regs

  regs.w.ax=0x1510;								// Send Device Driver Request Interrupt No 
  regs.w.cx=drive;
  sregs.es=FP_SEG(&IOCTLI);
  regs.w.bx=FP_OFF(&IOCTLI);

  int86x(0x2f,&regs, &regs, &sregs);
 }

/************************************************************************
*																		*
*							 AudioPaused16								*
*																		*
*  PARAMETERS: int drive;												*
*																		*
*  DESCRIPTION: Checks if CD-DA is paused								*
*																		*
*  RETURNS: 1 if paused, 0 otherwise									*
*																		*
************************************************************************/

short AudioPaused16(short drive)
 {
  short IsPaused;

  memset(&sregs,0,sizeof(sregs));
  memset(&regs,0,sizeof(regs)) ;

  StatusInfo=(struct AudStatHdr *)malloc(sizeof(StatusTable));

  StatusInfo->Command_Code=15;

  //Set Up Device Driver Header
  
  IOCTLI.DDH.DDH_Length=13;						// Maybe 13 + 7 ??? But then again maybe not!
  IOCTLI.DDH.Subunit_Code=0;					// Check This !!
  IOCTLI.DDH.Command_Code=3;					// Code for IOCTL Input 
  IOCTLI.DDH.Status=0;							// Set on exit by device driver 

  //Set Up Rest Of IOCTL Block

  IOCTLI.MediaDescrip=0;
  IOCTLI.TransAddrOff=FP_OFF(StatusInfo);
  IOCTLI.TransAddrSeg=FP_SEG(StatusInfo);					
  IOCTLI.Bytes2Trans=7;
  IOCTLI.StartSectNo=0;
  IOCTLI.ID=0;

  //Set Up Regs

  regs.w.ax=0x1510;								// Send Device Driver Request Interrupt No 
  regs.w.cx=drive;
  sregs.es=FP_SEG(&IOCTLI);
  regs.w.bx=FP_OFF(&IOCTLI);

  int86x(0x2f,&regs, &regs, &sregs);

  IsPaused=(unsigned short)StatusInfo->PausedStatus;

  IsPaused=IsPaused & 1;

  return(IsPaused);
 }

/************************************************************************
*																		*
*								 PlayDA16								*
*																		*
*  PARAMETERS: int track, int drive 									*
*								   										*
*  DESCRIPTION: Go on, guess.....										*
*																		*
*  RETURNS: Sod All														*
*																		*
*************************************************************************/

void PlayDA16(struct CdlPos *CurrTrk, short drive)
 {
  unsigned long From;
  unsigned long To;
  unsigned char FromMin;
  unsigned char FromSec;
  unsigned char FromFrame;
  unsigned char ToMin;
  unsigned char ToSec;
  unsigned char ToFrame;


  memset(&sregs,0,sizeof(sregs));
  memset(&regs,0,sizeof(regs));

  PlayTable.DDH.DDH_Length=13;						// Maybe 13 + 7 ??? But then again maybe not!
  PlayTable.DDH.Subunit_Code=0;					// Check This !!
  PlayTable.DDH.Command_Code=132;					// Code for Play Audio 
  PlayTable.DDH.Status=0;							// Set on exit by device driver 

  PlayTable.Mode=0;

  FromMin=BCDToBin16(CurrTrk->minute);
  FromSec=BCDToBin16(CurrTrk->second);
  FromFrame=BCDToBin16(CurrTrk->sector);

  From=RedBookToSectors16(FromFrame,FromSec,FromMin);

  CurrTrk++;

  ToMin=BCDToBin16(CurrTrk->minute);
  ToSec=BCDToBin16(CurrTrk->second);
  ToFrame=BCDToBin16(CurrTrk->sector);

  To=RedBookToSectors16(ToFrame,ToSec,ToMin);

  CurrTrk--;

  PlayTable.Start=From;
 
  PlayTable.Sectors2Read=To-From;

  regs.w.ax=0x1510;
  regs.w.cx=drive;
  sregs.es=FP_SEG(&PlayTable);
  regs.w.bx=FP_OFF(&PlayTable);

  int86x(0x2f,&regs, &regs, &sregs);
 }

 /***********************************************************************
 *																		*
 *							 ResumeAudio16								*
 *																		*
 *  PARAMETERS: int drive												*
 *																		*
 *  DESCRIPTION: restarts paused DA play								*
 *																	    *
 *  RETURNS: Sod All													*
 *																		*
 ***********************************************************************/

 void ResumeAudio16(short drive)
  {
   memset(&sregs,0,sizeof(sregs));
   memset(&regs,0,sizeof(regs));

   ResumeHdr.DDH_Length=13;
   ResumeHdr.Subunit_Code=0;
   ResumeHdr.Command_Code=136;
   ResumeHdr.Status=0;

   regs.w.ax=0x1510;
   regs.w.cx=drive;
   sregs.es=FP_SEG(&ResumeHdr);
   regs.w.bx=FP_OFF(&ResumeHdr);
								 
   int86x(0x2f,&regs, &regs, &sregs);
  }

 /***********************************************************************
 *																		*
 *								 StopDA16								*
 *																		*
 *  PARAMETERS:	int drive												*
 *																		*
 *  DESCRIPTION: Stops Audio by pausing twice (don't ask...) or once	*
 *				 if audio already paused.								*
 *																		*
 *  RETURNS: Sod All													*
 *																		*
 ***********************************************************************/

 void StopDA16(short drive)
  {
   if (AudioPaused16==1)
	{
	 PauseDA16(drive);
	}
   else
	{
	 PauseDA16(drive);
	 PauseDA16(drive);
	}
  }

/************************************************************************
*																		*
*							RedBookToSectors16							*
*																		*
*  PARAMETERS:	char RBFrame, char RBSec, int RBMin						*
*																		*
*  DESCRIPTION: Converts Red Book Address To Sector Address				*
*																		*
*  RETURNS: unsigned long int Sector Address							*
*																		*
***********************************************************************/

unsigned long RedBookToSectors16(unsigned char RBFrame, unsigned char RBSec, unsigned char RBMin)
 {
  unsigned long SectAddr;
  
  SectAddr=0;
  SectAddr=(((unsigned long ) RBMin)*60)*75;
  SectAddr+=((unsigned long ) RBSec)*75;
  SectAddr+=((unsigned long ) RBFrame)-150;
  return(SectAddr);	
 }


/************************************************************************
*																		*
* 								BCDToBin16								*
*																		*
*  PARAMETERS: unsigned char BCD  (packed BCD byte)						*
*																		*
*  DESCRIPTION:	converts from BCD to binary								*
*																		*
*  RETURNS: unsigned char in binary										*
*																		*
************************************************************************/

 unsigned char BCDToBin16(unsigned char BCD)
  {
   unsigned char Bin;

   Bin=(BCD & 0xf0)/16*10;
   Bin+=(BCD & 0xf);
   return(Bin);
  }

/************************************************************************
*																		*
*							 BinToBCD16									*
*																		*
*  PARAMETERS: unsigned char Bin										*
*																		*
*  DESCRIPTION:	converts binary to packed BCD							*
*																		*
*  RETURNS:	unsigned char in binary										*
*																		*
************************************************************************/																		

unsigned char	BinToBCD16(unsigned char Bin)
 {
  unsigned char BCD;
  unsigned char Temp;

  Temp = Bin/10;
  BCD=Temp*16;
  BCD+=(Bin-(Temp*10));
  return(BCD);
 }

/************************************************************************
*																		*
*								PauseDA16									*
*																		*
*  PARAMETERS: int drive												*
*																		*
*  DESCRIPTION: Pauses Audio Play										*
*																		*
*  RETURNS: Sod All														*
*																		*
************************************************************************/

void PauseDA16(int drive)
 {
  memset(&sregs,0,sizeof(sregs));
  memset(&regs,0,sizeof(regs));

  PauseHdr.DDH_Length=13;
  PauseHdr.Subunit_Code=0;
  PauseHdr.Command_Code=133;
  PauseHdr.Status=0;

  regs.w.ax=0x1510;
  regs.w.cx=drive;
  sregs.es=FP_SEG(&PauseHdr);
  regs.w.bx=FP_OFF(&PauseHdr);

  int86x(0x2f,&regs, &regs, &sregs);
 }

