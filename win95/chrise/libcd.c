/************************************************************************
*																		*
*																		*
*************************************************************************

/************************************************************************
*																		*
*			Les Includes												*
*																		*
************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <i86.h>
#include <dos.h>
#include "libcd.h"

void	GetCdProtectionInfo();
unsigned long Red2Secs(unsigned long RedValue);
void BlackPig();
unsigned long	pugwash[7]={23656, 24159, 22940, 23676, 23772, 23922, 24485};
char	thieving_git=0;
/************************************************************************
*																		*
*			Los Defines													*
*																		*
************************************************************************/

/************************************************************************
*																		*
*			Les Enums													*
*																		*
************************************************************************/


/************************************************************************
*																		*
*			Das Variables												*
*																		*
************************************************************************/

char 				CdPaused;

CDDIStru			DiskInfo;
CDTIStru			TrackInfo;
CDResetStru			ResetCD;
CDCmdStru			CDCmdBlock;
TNOInfo				TrackInfoTable;
CDStatusStru		CDStatusTable;
QChanStru			QChanInfo;

short int   		CurrTrkNo;
unsigned char 		DriveID = 0;
short int 			Highest;
short int 			Lowest;

struct  CdlPos 		*CurrTrk;
struct  CdlPos 		*SearchPtr;
struct  CdlPos 		*CDTable;

union  				REGS	 inregs, outregs;
struct 				SREGS 	 insegregs;	

unsigned short 		LowMemSeg = 0;
DPMI_CALLREGS 		LibCDdr;

void far 			*fh1;
char 				*LowMem;

DWORD				SegmentA;
DWORD				OffsetA;
unsigned long 		AddressA;

unsigned short 		DIOffset = 0;
unsigned short 		TIOffset = sizeof(CDDIStru);
unsigned short 		ROffset  = sizeof(CDTIStru) + sizeof(CDDIStru);
unsigned short 		CBOffset = sizeof(CDResetStru) + sizeof(CDTIStru) + sizeof(CDDIStru);
unsigned short 		ASOffset = sizeof(CDCmdStru) + sizeof(CDResetStru )+ sizeof(CDTIStru) + sizeof(CDDIStru);
unsigned short		QCOffset = sizeof(CDStatusStru) + sizeof(CDCmdStru) + sizeof(CDResetStru )+ sizeof(CDTIStru) + sizeof(CDDIStru);

unsigned long 		StartSector = 0;
unsigned long 		EndSector = 0;

unsigned char 		LibCDTrack;
unsigned char 		CDOk;
unsigned char		DAPlaying;

/************************************************************************
*																		*
*			Da Code														*
*																		*
************************************************************************/

/************************************************************************
*																		*
*			Los Functions												*
*																		*
************************************************************************/


/************************************************************************
*																		*	
* 							D32DosMemAlloc								*
*																		*
************************************************************************/

void *D32DosMemAlloc (DWORD size, WORD *selector)
 {
 #ifdef PORT_DA
  inregs.x.eax = 0x0100;						// DPMI allocate DOS memory 
  inregs.x.ebx = (size + 15) >> 4;				// Number of paragraphs requested 
  int386 (0x31, &inregs, &outregs);
   
  if (outregs.x.cflag)
   return ((DWORD) 0);							// Failed 

  if (selector)
   *selector = outregs.w.dx;
#endif

  return (void *) ((outregs.x.eax & 0xFFFF) << 4);
 }
 
/************************************************************************
*																		*
*							 BinToBCD									*
*																		*
*  PARAMETERS: unsigned char Bin										*
*																		*
*  DESCRIPTION:	converts binary to packed BCD							*
*																		*
*  RETURNS:	unsigned char in binary										*
*																		*
************************************************************************/																		

unsigned char	BinToBCD(unsigned char Bin)
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
* 							CDInit										*
*																		*
************************************************************************/
void CdInit()
 {
#ifdef PORT_DA

  /* Allocate Dos Low memory for CD Audio Data */
  /* I have to allocate so much mem for so little data coz' */
  /* the allocation interrput doesn't work reliably. */
  /* ie. it returns a pointer to environment memory */

  if (! (LowMem = D32DosMemAlloc (1024, &LowMemSeg)))
   {
   //  Couldn't Alloc Low Memory for CD Audio 
    CDOk = 1;
   }

  if (! CDOk)
   {
    Reset();

    //Get Drive ID 

    inregs.x.eax = 0x300;						// DPMI simulate real mode interrupt 
    inregs.x.ebx = 0x2f;
    inregs.x.ecx = 0;
    fh1 = (void far *) &LibCDdr;
    _fmemset (fh1, 0, sizeof(DPMI_CALLREGS));
 	
    LibCDdr.eax = INIT_MP_INT;

    insegregs.ds = 0;
    insegregs.es = FP_SEG(fh1);
    inregs.x.edi = FP_OFF(fh1);
    int386x (0x31, &inregs, &inregs, &insegregs);

    DriveID = LibCDdr.ecx;

    GetDiskInfo();
//	  GetCdProtectionInfo();
	  BlackPig();
	DAPlaying=0;
    CDOk = 0; 

   }
#endif
 }


void	GetCdProtectionInfo()
{
#if WE_NEED_TO_GET_THE_VALUES_SO_DEBUG_IT
	char	track;
	unsigned long	t1;
	unsigned long	t2;

		
	for (track=2;track<9;track++)
	{
		GetTrackInfo(track);
		t1=Red2Secs(TrackInfo.StartAddress);
		GetTrackInfo(track+1);
		t2=Red2Secs(TrackInfo.StartAddress);
		pugwash[track-2]=t2-t1;
	}
#endif
}	

void BlackPig()
{
	char	track;
	unsigned long	t1;
	unsigned long	t2;
	unsigned long	temp;

	thieving_git=0;
	return;
#ifdef PC_DEMO
	return;
#endif
	for (track=2;track<9;track++)
	{
		GetTrackInfo(track);
		t1=Red2Secs(TrackInfo.StartAddress);
		GetTrackInfo(track+1);
		t2=Red2Secs(TrackInfo.StartAddress);
		temp=t2-t1;
		if (pugwash[track-2]!=temp)
		{
			thieving_git=1;
		}
	}
}


/************************************************************************
*																		*	
* 									SendCDCommand						*
*																		*
************************************************************************/


void SendCDCommand(long Address, long Size, unsigned char CommandCmnd)
 {
  // Set up Command Data 
#ifdef PORT_DA
  CDCmdBlock.ReqHdrLength	= 13;
  CDCmdBlock.SubUnit		= 0;
  CDCmdBlock.CommandCode	= CommandCmnd;
  CDCmdBlock.Status			= 0;
  CDCmdBlock.Reserved1		= 0;
  CDCmdBlock.Reserved2		= 0;

  CDCmdBlock.AddressMode	= ADDR_HSG;
  CDCmdBlock.BeginPointer	= (void *)Address;
  CDCmdBlock.Length			= Size;
  CDCmdBlock.SSNumber		= 0;
  CDCmdBlock.VolIDPtr		= 0;

  // Copy Command data to LowMem 

  _fmemcpy ((char __far *) (LowMem+CBOffset), (void __far *) &CDCmdBlock, sizeof(CDCmdStru));

  // Send the command 
  inregs.x.eax = 0x300;						// DPMI simulate real mode interrupt
  inregs.x.ebx = 0x2f;
  inregs.x.ecx = 0;
  fh1 = (void far *) &LibCDdr;
  _fmemset (fh1, 0, sizeof(DPMI_CALLREGS));

  OffsetA = D32RealOff(LowMem+CBOffset);
  SegmentA = D32RealSeg(LowMem+CBOffset);

  LibCDdr.eax = CDREQ_MP_INT;
  LibCDdr.ebx = (long)OffsetA;
  LibCDdr.es = SegmentA;
  LibCDdr.ecx = DriveID;

  insegregs.ds = 0;
  insegregs.es = FP_SEG(fh1);
  inregs.x.edi = FP_OFF(fh1);
  int386x (0x31, &inregs, &inregs, &insegregs);
#endif
 }
 
/************************************************************************
*																		*	
* 								GetDiskInfo								*
*																		*
************************************************************************/

void GetDiskInfo()
 {
  // Fills the DiskInfo with LowTrack, HiTrack and LeadOut Info 

#ifdef PORT_DA
  DiskInfo.CommandCode = GET_AUDIO_DISKINFO;

  // Copy DiskInfo Data into Low Memory 
	
  _fmemcpy ((char __far *) LowMem, (void __far *) &DiskInfo, sizeof(CDDIStru));

  OffsetA = D32RealOff(LowMem);
  SegmentA = D32RealSeg(LowMem);
  AddressA = (SegmentA<<16) | OffsetA;


  SendCDCommand(AddressA, sizeof(CDDIStru), READ_IOCTL_COMMAND);

  // Copy New Low Memory Data into DiskInfo

  _fmemcpy ((void __far *) &DiskInfo, (char __far *) LowMem, sizeof(CDDIStru));
  DAPlaying=0;
  //printf("FirstTrack %d\n",DiskInfo.LoTrack);
  //printf("LastTrack %d\n",DiskInfo.HiTrack);
  //getch();
#endif
 }

/************************************************************************
*																		*		
* 								GetTrackInfo							*
*																		*
************************************************************************/

void GetTrackInfo(char Track)
 {

#ifdef PORT_DA
  TrackInfo.CommandCode = GET_AUDIO_TRACKINFO;
  TrackInfo.TrackNumber = Track;
	
  // Copy TrackInfo Data into Low Memory

  _fmemcpy ((char __far *) LowMem+TIOffset, (void __far *) &TrackInfo, sizeof(CDTIStru));

  OffsetA = D32RealOff(LowMem+TIOffset);
  SegmentA = D32RealSeg(LowMem+TIOffset);
  AddressA = (SegmentA<<16) | OffsetA;

  SendCDCommand(AddressA, sizeof(CDTIStru), READ_IOCTL_COMMAND);

  // Copy New Low Memory Data into TrackInfo 

  _fmemcpy ((void __far *) &TrackInfo, (char __far *) LowMem+TIOffset, sizeof(CDTIStru));
  DAPlaying=0;
#endif
 }

/************************************************************************
*																		*		
* 										GetAudioAddr					*
*																		*
************************************************************************/

void GetAudioAddr(char Track)
 {
#ifdef PORT_DA
  TrackInfo.CommandCode = GET_AUDIO_TRACKINFO;
  TrackInfo.TrackNumber = Track;
	
  // Copy TrackInfo Data into Low Memory 
  
    _fmemcpy ((char __far *) LowMem+TIOffset, (void __far *) &TrackInfo, sizeof(CDTIStru));

  OffsetA = D32RealOff(LowMem+TIOffset);
  SegmentA = D32RealSeg(LowMem+TIOffset);
  AddressA = (SegmentA<<16) | OffsetA;

  SendCDCommand(AddressA, sizeof(CDTIStru), READ_IOCTL_COMMAND);

   //Copy New Low Memory Data into TrackInfoTable

  _fmemcpy ((void __far *) &TrackInfoTable, (char __far *) LowMem+TIOffset, sizeof(CDTIStru));
#endif
 }

/************************************************************************
*																		*	
*										 Reset							*
*																		*
************************************************************************/

void Reset()
 {
#ifdef PORT_DA
  ResetCD.CommandCode = CD_RESET_COMMAND;

  _fmemcpy ((char __far *) LowMem+ROffset, (void __far *) &ResetCD, sizeof(CDResetStru));

  OffsetA = D32RealOff(LowMem+ROffset);
  SegmentA = D32RealSeg(LowMem+ROffset);
  AddressA = (SegmentA<<16) | OffsetA;

  SendCDCommand(AddressA, sizeof(CDResetStru), WRITE_IOCTL_COMMAND);
  DAPlaying=0;
#endif
 }
 
/************************************************************************
*																		*	
* 										Red2HSG							*
*																		*
************************************************************************/

unsigned long Red2HSG(unsigned long RedValue)
 {
  unsigned long Mins;
  unsigned long Secs;
  unsigned long PlusFrames;

  PlusFrames = RedValue & 0x000000ff;

  Secs = ((RedValue & 0x0000ff00) >> 8);

  Mins = ((RedValue & 0x00ff0000) >> 16);

  return((Mins*60*75)+(Secs*75)+PlusFrames-150);
 }
 
unsigned long Red2Secs(unsigned long RedValue)
 {
  unsigned long Mins;
  unsigned long Secs;
  unsigned long PlusFrames;

  PlusFrames = RedValue & 0x000000ff;

  Secs = ((RedValue & 0x0000ff00) >> 8);

  Mins = ((RedValue & 0x00ff0000) >> 16);

  return((Mins*60*75)+(Secs*75)+PlusFrames-150);
 }


/************************************************************************
*																		*
*								BCDToBin								*
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
*							RedBookToSectors							*
*																		*
*  PARAMETERS:	char RBFrame, char RBSec, int RBMin						*
*																		*
*  DESCRIPTION: Converts Red Book Address To Sector Address				*
*																		*
*  RETURNS: unsigned long int Sector Address							*
*																		*
***********************************************************************/

unsigned long RedBookToSectors(unsigned char RBFrame, unsigned char RBSec, unsigned char RBMin)
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
* 									CDPaused							*
*																		*
************************************************************************/

char CDPaused()
// Returns 1 if DA paused, 0 otherwise 
 {
 #ifdef PORT_DA
  CDStatusTable.CommandCode = GET_AUDIO_STATUSINFO;

  // Copy Status Table Data Into Low Memory

  _fmemcpy ((char _far *) LowMem+ASOffset, (void _far*) &CDStatusTable, sizeof(CDStatusStru));

  OffsetA =	D32RealOff(LowMem+ASOffset);
  SegmentA = D32RealSeg(LowMem+ASOffset);
  AddressA = (SegmentA<<16) | OffsetA;

  SendCDCommand(AddressA, sizeof(CDStatusStru), READ_IOCTL_COMMAND);

  // Copy New Low Memory Data into Track Info

  _fmemcpy ((void __far *) &CDStatusTable, (char __far *) LowMem+ASOffset, sizeof(CDStatusStru));
#endif
  return(CDStatusTable.AudioStatus & 1);
 }

/************************************************************************
*																		*	
* 									CDResume							*
*																		*
************************************************************************/

void CDResume()
 {
 SendCDCommand(0, 0, CD_RESUME_COMMAND);
 CdPaused = 0;
 DAPlaying=1;
 }

/************************************************************************
*																		*	
* 									CDPause								*
*																		*
************************************************************************/

void CDPause()
 {
  
  if (CDPaused != 1)
   {						 
    SendCDCommand(0, 0, CD_PAUSE_COMMAND);
	CdPaused =1;
	DAPlaying=0;
   }
   
 //CdMusicStop();

 }

/************************************************************************
*																		*	
* 									CDStop								*
*																		*
************************************************************************/

void CDStop()
 {
  if (CdPaused !=1 )
   {						 
    SendCDCommand(0, 0, CD_PAUSE_COMMAND);
    SendCDCommand(0, 0, CD_PAUSE_COMMAND);
	CdPaused=0;
	//CdMusicStop();
 	//CdmusicStop();
	//CdPaused=0;
   }
  else
   {
	SendCDCommand(0, 0, CD_PAUSE_COMMAND);
	CdPaused=0;
	//CdMusicStop();
	//CdPaused=0;
    DAPlaying=0;
   }
 }

/************************************************************************
*																		*	
* 									CDPlay								*
*																		*
************************************************************************/

/* If this takes too much time go back to using my caching system */
/*  My stuff must be changed to allow for last track bug */

void CDPlay(short int CurrTrackNo)
 {
  CurrTrackNo--;
  GetTrackInfo((char) CurrTrackNo);

  if (TrackInfo.Control & 0x40)
   {
	// Not an Audio Track  
	CDOk = 1;
	return;
   }
  StartSector = Red2HSG(TrackInfo.StartAddress);

  if (CurrTrackNo > DiskInfo.HiTrack)
   {
    // Not that many tracks on the Disk  
    CDOk = 1;
    return;
   }

  if (CurrTrackNo == DiskInfo.HiTrack)
   {
	EndSector = Red2HSG(DiskInfo.LeadOut);
   }
  else
   {
	GetTrackInfo((char) CurrTrackNo+1);
	EndSector = Red2HSG(TrackInfo.StartAddress) - 150;
   }
	EndSector = Red2HSG(DiskInfo.LeadOut);

  SendCDCommand(StartSector, EndSector-StartSector, CD_PLAY_COMMAND);
  CDOk = 0;
  DAPlaying =1;
  CurrTrackNo++;
  
  //CdMusicPlay((uint16) CurrTrackNo, 0);
 }

/************************************************************************
*																		*
* 								CDLoopCheck								*
*																		*
*  PARAMETERS: None														*
*																		*
*  DESCRIPTION: Checks for end of DA track and forces loop				*
*																		*
*  RETURNS: Nowt														*
*																		*
************************************************************************/

void CDLoopCheck()
 {
  unsigned char MinsPlayed;
  unsigned char SecsPlayed;
  unsigned long TotSecs;

#ifdef PORT_DA
  if (DAPlaying==1)
   {
    QChanInfo.CommandCode = GET_Q_CHANNEL_INFO;

    //Copy info to low mem

    _fmemcpy ((char __far *) LowMem+QCOffset, (void __far *) &QChanInfo, sizeof(QChanStru));
    OffsetA =	D32RealOff(LowMem+QCOffset);
    SegmentA = D32RealSeg(LowMem+QCOffset);
    AddressA = (SegmentA<<16) | OffsetA;
  
    SendCDCommand(AddressA, sizeof(QChanStru), READ_IOCTL_COMMAND);

    // Copy New Low Memory Data into QChanInfo

    _fmemcpy ((void __far *) &QChanInfo, (char __far *) LowMem+QCOffset, sizeof(QChanStru));

	TotSecs=EndSector+150;
    TotSecs/=75;
	MinsPlayed = (unsigned char)TotSecs/60;
    SecsPlayed = (unsigned char)EndSector%60;
  
    if ((MinsPlayed<=QChanInfo.DiskMin) && (SecsPlayed<=QChanInfo.DiskSec))
     {
      CDStop();
      CDPlay(CurrTrkNo); 
     }
   }
#endif
 }
/************************************************************************
*																		*
* 								CdControl								*
*																		*
*  PARAMETERS: unsigned char Cmd, unsigned char *parm1,					*
*              unsigned char *parm2										*
*																		*
*  DESCRIPTION: Master control for CD functions							*
*																		*
*  RETURNS: 1 if successful												*
*																		*
************************************************************************/

int CdControl(unsigned char Cmd, struct CdlPos *parm1, unsigned char  *parm2)
 {
 #ifdef PORT_DA
  switch (Cmd)
   {
	case (CdlPlay):
	 {
//	  if (CdPaused==1)
	  if (0)
	   {
	    CDResume();
	   }
	  else
	   {
	    CDPlay(CurrTrkNo);
	   }
	  break;
	 }
  	case (CdlPause):
  	 {
	  CDPause();	
	 }
	break;
	case (CdlStop):
	 {
	  CDStop();
	 }
	break;
	case (CdlSetloc): 
	 {
	  SearchPtr=CDTable;
	  CurrTrkNo=1;
	  CurrTrk = SearchPtr;

	  while (((SearchPtr->minute)!=(parm1->minute)) && ((SearchPtr->second)!=(parm1->second)) && ((SearchPtr->sector)!=(parm1->sector)))
	   {
		CurrTrkNo++;
		SearchPtr++;
		CurrTrk = SearchPtr;
 	   }
	  CdPaused = 0;
	 }
	 break;
	case (CdlSeekL): 
	 {
	  SearchPtr=CDTable;
	  CurrTrkNo=1;
	  CurrTrk = SearchPtr;

	  while (((SearchPtr->minute)!=(parm1->minute)) && ((SearchPtr->second)!=(parm1->second)) && ((SearchPtr->sector)!=(parm1->sector)))
	   {
		CurrTrkNo++;
		SearchPtr++;
		CurrTrk = SearchPtr;
 	   }
	  CdPaused = 0;
	 }
	 break;
	case (CdlSeekP):
	 {
	  SearchPtr=CDTable;
	  CurrTrkNo=1;

	  while (((SearchPtr->minute)!=(parm1->minute)) && ((SearchPtr->second)!=(parm1->second)) && ((SearchPtr->sector)!=(parm1->sector)))
	   {
	    CurrTrkNo++;
		SearchPtr++;
		CurrTrk = SearchPtr;
	   }
	  CdPaused = 0;
	 }
	 break;


    }
#endif
   return(1);
  }

/************************************************************************
*																		*
* 								CdGetToc2								*
*																		*
*  PARAMETERS: struct AudioTrkRec *TrkOne (unused but kept for pseudo-	*
*				compatability											*
*																		*
*  RETURNS:	No of Audio Tracks											*
*																		*
************************************************************************/

int CdGetToc2(int sess, struct CdlPos *table)
 {
  short int	TrackNo;

  short int RBMin;
  short int RBSec;
  short int RBFrame;

#ifdef PORT_DA
  CDTable= table;

  TrackNo=0;

  // Get First and Last Track No's, plus Lead Out Address (total playing time)

  GetDiskInfo();

  RBMin=(DiskInfo.LeadOut & 0xFF0000)>>16;
  RBSec=(DiskInfo.LeadOut & 0xFF00)>>8;
  RBFrame=(DiskInfo.LeadOut & 0xFF);

  // Write total playing time to first element in table...which is zero(silly C rubbish, not like Pascal oh no)

  table[TrackNo].minute=BinToBCD(RBMin);
  table[TrackNo].second=BinToBCD(RBSec);
  table[TrackNo].sector=BinToBCD(RBFrame);

  //Get Audio Info for DA Tracks

  Lowest = DiskInfo.LoTrack;
  Highest= DiskInfo.HiTrack;

  for(TrackNo=Lowest;TrackNo<=Highest;TrackNo++)
   {
	GetAudioAddr((char)TrackNo);
	table[TrackNo].minute=BinToBCD(TrackInfoTable.Min);
	table[TrackNo].second=BinToBCD(TrackInfoTable.Sec);
	table[TrackNo].sector=BinToBCD(TrackInfoTable.Frame);
   }

#endif
  return((int) --TrackNo);
 }


