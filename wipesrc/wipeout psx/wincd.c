#include <stdio.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>
#include <mmsystem.h>
#include "libcd.h"

extern HWND hwnd;
extern int ntoc;

BOOLEAN dosound = FALSE;
short int   		CurrTrkNo;
char	thieving_git=0;
short starttrack;
short endtrack;
UINT cdDevNum;

typedef struct trackinfo
{
	char startmins;
	char startsecs;
	char endmins;
	char endsecs;
} trackinfo;

trackinfo TrackDetails[10];

MCI_STATUS_PARMS mci_status_parms;
MCI_SET_PARMS mci_set_parms;
UINT wDeviceID;
UINT auxDeviceID;
DWORD retval;
CDDIStru			DiskInfo;

void doStuff()
{
	UINT numDevs, i;
	AUXCAPS caps;

	numDevs = auxGetNumDevs();
	for (i = 0; i < numDevs; i++)
	{
		auxGetDevCaps(i, &caps, sizeof(AUXCAPS));
	}
}



void *D32DosMemAlloc (DWORD size, WORD *selector)
 {
 return (void *)0;
 }
 																		

unsigned char BinToBCD(unsigned char Bin)
 {
  return(0);
 }
 
/************************************************************************
*																		*	
* 							CDInit										*
*																		*
************************************************************************/


void CdInit()
{

	int fp;
	char info[10];
	char theChar;
	short i;
	int tn;
	UINT numDevs;
	AUXCAPS caps;
	MCI_OPEN_PARMS mci_open_parms;

	if (dosound)
		return;


	mci_open_parms.lpstrDeviceType = "cdaudio";
	if (retval = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE, 
		(DWORD)(LPVOID) &mci_open_parms))
	{
		return;//broke :(
	}
	wDeviceID = mci_open_parms.wDeviceID;
	
	mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
	if (retval = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, 
		(DWORD)(LPVOID) &mci_set_parms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
		return;//broke :(
	}

	mci_status_parms.dwItem = MCI_STATUS_READY;
	if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, 
		(DWORD)(LPVOID) &mci_status_parms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
		return;
	}
	if (mci_status_parms.dwReturn)
		dosound = TRUE;

	fp = open("startup.ini", O_RDONLY);
	if (fp >= 0)
	{
		read(fp, &theChar, 1);
		i = 0;

		while (theChar != '\n' && theChar != '\0')
		{
			info[i++] = theChar;
			read(fp, &theChar, 1);
		}
		info[i] = '\n';

		if (!strncmp(info, "NONE", 4))
		{
			dosound = FALSE;
			return;
		}
		starttrack = atoi(info);

		read(fp, &theChar, 1);
		i = 0;

		while (theChar != '\n' && theChar != '\0')
		{
			info[i++] = theChar;
			read(fp, &theChar, 1);
		}
		info[i] = '\n';

		endtrack = atoi(info);
		close(fp);
	} else {
		dosound = FALSE;
		return;
	}

	CurrTrkNo = starttrack;

	mci_set_parms.dwTimeFormat = MCI_FORMAT_MSF;
	if (retval = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT, 
		(DWORD)(LPVOID) &mci_set_parms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
		return;//broke :(
	}

	for (tn = starttrack; tn <= endtrack; tn++)
	{
		
	   //read start points of tracks
		mci_status_parms.dwItem = MCI_STATUS_POSITION;
		mci_status_parms.dwTrack = tn;
		if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
			(DWORD)(LPVOID) &mci_status_parms))
		{
			mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
			return;
		}

		TrackDetails[tn].startmins = MCI_MSF_MINUTE(mci_status_parms.dwReturn);
		TrackDetails[tn].startsecs = MCI_MSF_SECOND(mci_status_parms.dwReturn);

		//find length of track
		mci_status_parms.dwItem = MCI_STATUS_LENGTH;
		mci_status_parms.dwTrack = tn;
		if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
			(DWORD)(LPVOID) &mci_status_parms))
		{
			mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
			return;
		}

		TrackDetails[tn].endmins = MCI_MSF_MINUTE(mci_status_parms.dwReturn)
			+ TrackDetails[tn].startmins;
		TrackDetails[tn].endsecs = MCI_MSF_SECOND(mci_status_parms.dwReturn)
			+ TrackDetails[tn].startsecs;
		if (TrackDetails[tn].endsecs >= 60)
		{
			TrackDetails[tn].endmins += 1;
			TrackDetails[tn].endsecs -= 60;
		}
	}


	numDevs = auxGetNumDevs();
	for (i = 0; i < numDevs; i++)
	{
		auxGetDevCaps(i, &caps, sizeof(AUXCAPS));
		if (caps.dwSupport & AUXCAPS_VOLUME)
			auxDeviceID = i;
	}
}


void	GetCdProtectionInfo()
{
}	

void BlackPig()
{
}


void SendCDCommand(long Address, long Size, unsigned char CommandCmnd)
 {
  
 }
 
/************************************************************************
*																		*	
* 								GetDiskInfo								*
*																		*
************************************************************************/

void GetDiskInfo()
 {
  
 	DiskInfo.CommandCode = '0';
	DiskInfo.LoTrack = 1;
	DiskInfo.HiTrack = (unsigned char)numTracks();
	DiskInfo.LeadOut = 0;
 }


/************************************************************************
*																		*		
* 								GetTrackInfo							*
*																		*
************************************************************************/

void GetTrackInfo(char Track)
 {

 }


void GetAudioAddr(char Track)
 {
 }

/************************************************************************
*																		*	
*										 Reset							*
*																		*
************************************************************************/

void Reset()
 {
	//mciSendString("stop cdaudio", NULL, NULL, NULL);
 }
 
int GetCurrTrackNo()
{
	/*static char returnstring[80];
	LPSTR response = returnstring;
	if (dosound)
	{
		mciSendString("status cdaudio current track", response, lstrlen(response), NULL);
		CurrTrkNo = atoi(returnstring);	
		return(CurrTrkNo);  
		return CurrTrkNo; 
	}*/
	return 1;
}

/************************************************************************
*																		*	
* 										Red2HSG							*
*																		*
************************************************************************/

unsigned long Red2HSG(unsigned long RedValue)
 {
 return 0;
 }
 
unsigned long Red2Secs(unsigned long RedValue)
 {
 return 0;
 }


/************************************************************************
*																		*
*								BCDToBin								*
*																		*
************************************************************************/

 unsigned char BCDToBin16(unsigned char BCD) {
return (0);}
 unsigned long RedBookToSectors(unsigned char RBFrame, unsigned char RBSec, unsigned char RBMin)
 {
 return(0);	
 }

/************************************************************************
*																		*
* 									CDPaused							*
*																		*
************************************************************************/

char CDPaused()
// Returns 1 if DA paused, 0 otherwise 
 {
	if (dosound)
	{
		mci_status_parms.dwItem = MCI_STATUS_MODE;
		if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, 
			(DWORD)(LPVOID) &mci_status_parms))
		{
			//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
			return(0);
		}
		if (mci_status_parms.dwReturn == MCI_MODE_PAUSE)
			return('1');
		else
			return('0');
	}
	return('0');
 }

/************************************************************************
*																		*	
* 									CDResume							*
*																		*
************************************************************************/

void CDResume()
 {
	if (dosound)
	{
 		if (retval = mciSendCommand(wDeviceID, MCI_RESUME, NULL, NULL))
		{
			//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
			return;
		}
	} 
}

/************************************************************************
*																		*	
* 									CDPause								*
*																		*
************************************************************************/

void CDPause()
 {
	if (dosound)
	{
		if (retval = mciSendCommand(wDeviceID, MCI_PAUSE, NULL, NULL))
		{
			//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
			return;
		}
	}
 }

/************************************************************************
*																		*	
* 									CDStop								*
*																		*
************************************************************************/

void CDStop()
 {
	if (dosound)
	{
  		if (retval = mciSendCommand(wDeviceID, MCI_STOP, NULL, NULL))
  		{
  			//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
  		}
	} 
}

void CDRestart()
{
	if (dosound)
	{
		CDStop();
		CDPlay(CurrTrkNo);
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
	MCI_PLAY_PARMS mci_play_parms;
	DWORD endpoint;

	if (dosound)
	{
		if(CurrTrackNo == 1)
			CurrTrackNo = (rand() % (ntoc - 1)) + 2;

		mci_play_parms.dwFrom =
			MCI_MAKE_MSF(
				TrackDetails[CurrTrackNo].startmins,
				TrackDetails[CurrTrackNo].startsecs,
				0);
		mci_play_parms.dwTo =
			MCI_MAKE_MSF(
				TrackDetails[CurrTrackNo].endmins,
				TrackDetails[CurrTrackNo].startsecs,
				0);
	
   		mci_play_parms.dwCallback = (DWORD)hwnd;

		if (retval = mciSendCommand(wDeviceID, MCI_PLAY, 
			MCI_FROM | MCI_TO | MCI_NOTIFY,
			(DWORD)(LPVOID) &mci_play_parms))
		{
			//mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
			return;
		}
		CurrTrkNo = CurrTrackNo;
		dosound = TRUE;
	} 
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
	if (dosound)
	{
		mci_status_parms.dwItem = MCI_STATUS_MODE;
		if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, 
			(DWORD)(LPVOID) &mci_status_parms))
		{
			mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
			return;
		}

		if (mci_status_parms.dwReturn == MCI_MODE_STOP)
			CDPlay(CurrTrkNo);
	}
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
 	if (dosound)
	{
  switch (Cmd)
   {
	case (CdlPlay):
	 {
	  if (CDPaused() == 1)
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
	  
 
	 }
	 break;
	case (CdlSeekL): 
	 {
	  
	 }
	 break;
	case (CdlSeekP):
	 {
	  
	 }
	 break;


    }
	}
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
int numTracks()
{
	int num;

	num = CdGetToc2(0, (struct CdlPos *)NULL);
	return num;
}


int CdGetToc2(int sess, struct CdlPos *table)
 {
	if (dosound)
	{
		mci_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
		if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, 
			(DWORD)(LPVOID) &mci_status_parms))
		{
			mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);	
			return(0);
		}

		if (mci_status_parms.dwReturn  < 2)
		{
			dosound = FALSE;//cd in but no tracks
			return 2;
		}

		return(mci_status_parms.dwReturn);
	}
	return 0;
}

void CdSetVol(long newVol)
{
	UINT ret;
	long theVol;

return;
	newVol = newVol * (32000 / 218);
	theVol = (newVol << 16) + newVol;
			
	//ret = auxSetVolume(wDeviceID, theVol);
	ret = auxSetVolume(auxDeviceID, theVol);
}

