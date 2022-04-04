#define STRICT
#include <stdio.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <fcntl.h>
#include <io.h>
#include <Windows.h>
#include <mmsystem.h>
#include "\wipesrc\win95\chrise\libcd.h"
#include "wincd.h"

extern HWND hwnd;
extern int32_t ntoc;

BOOLEAN dosound = FALSE;
int16_t int32_t CurrTrkNo;
int16_t starttrack;
int16_t endtrack;
UINT cdDevNum;

typedef struct trackinfo {
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
CDDIStru DiskInfo;

void doStuff() {
  UINT numDevs, i;
  AUXCAPS caps;

  numDevs = auxGetNumDevs();
  for (i = 0; i < numDevs; i++) {
    auxGetDevCaps(i, &caps, sizeof(AUXCAPS));
  }
}

void* D32DosMemAlloc(DWORD size, WORD* selector) {
  return (void*)0;
}

uint8_t BinToBCD(uint8_t Bin) {
  return (0);
}

/************************************************************************
*																		*
* 							CDInit										*
*																		*
************************************************************************/

void CdInit() {
  int32_t fp;
  char info[10];
  char theChar;
  int16_t i;
  int32_t tn;
  UINT numDevs;
  AUXCAPS caps;
  MCI_OPEN_PARMS mci_open_parms;

  if (dosound)
    return;

  mci_open_parms.lpstrDeviceType = "cdaudio";
  if (retval = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_TYPE,
                              (DWORD)(LPVOID)&mci_open_parms)) {
    return; //broke :(
  }
  wDeviceID = mci_open_parms.wDeviceID;

  mci_set_parms.dwTimeFormat = MCI_FORMAT_TMSF;
  if (retval = mciSendCommand(wDeviceID, MCI_SET, MCI_SET_TIME_FORMAT,
                              (DWORD)(LPVOID)&mci_set_parms)) {
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
    return; //broke :(
  }

  mci_status_parms.dwItem = MCI_STATUS_READY;
  if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
                              (DWORD)(LPVOID)&mci_status_parms)) {
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
    return;
  }
  if (mci_status_parms.dwReturn)
    dosound = TRUE;

  fp = open("startup.ini", O_RDONLY);
  if (fp >= 0) {
    read(fp, &theChar, 1);
    i = 0;

    while (theChar != '\n' && theChar != '\0') {
      info[i++] = theChar;
      read(fp, &theChar, 1);
    }
    info[i] = '\n';

    if (!strncmp(info, "NONE", 4)) {
      dosound = FALSE;
      return;
    }
    starttrack = atoi(info);

    read(fp, &theChar, 1);
    i = 0;

    while (theChar != '\n' && theChar != '\0') {
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
                              (DWORD)(LPVOID)&mci_set_parms)) {
    mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
    return; //broke :(
  }

  for (tn = starttrack; tn <= endtrack; tn++) {
    //read start points of tracks
    mci_status_parms.dwItem = MCI_STATUS_POSITION;
    mci_status_parms.dwTrack = tn;
    if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
                                (DWORD)(LPVOID)&mci_status_parms)) {
      mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
      return;
    }

    TrackDetails[tn].startmins = MCI_MSF_MINUTE(mci_status_parms.dwReturn);
    TrackDetails[tn].startsecs = MCI_MSF_SECOND(mci_status_parms.dwReturn);

    //find length of track
    mci_status_parms.dwItem = MCI_STATUS_LENGTH;
    mci_status_parms.dwTrack = tn;
    if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_TRACK,
                                (DWORD)(LPVOID)&mci_status_parms)) {
      mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
      return;
    }

    TrackDetails[tn].endmins = MCI_MSF_MINUTE(mci_status_parms.dwReturn) + TrackDetails[tn].startmins;
    TrackDetails[tn].endsecs = MCI_MSF_SECOND(mci_status_parms.dwReturn) + TrackDetails[tn].startsecs;
    if (TrackDetails[tn].endsecs >= 60) {
      TrackDetails[tn].endmins += 1;
      TrackDetails[tn].endsecs -= 60;
    }
  }

  numDevs = auxGetNumDevs();
  for (i = 0; i < numDevs; i++) {
    auxGetDevCaps(i, &caps, sizeof(AUXCAPS));
    if (caps.dwSupport & AUXCAPS_VOLUME)
      auxDeviceID = i;
  }
}

void GetCdProtectionInfo() {
}

void SendCDCommand(int32_t Address, int32_t Size, uint8_t CommandCmnd) {
}

/************************************************************************
*																		*
* 								GetDiskInfo								*
*																		*
************************************************************************/

void GetDiskInfo() {
  DiskInfo.CommandCode = '0';
  DiskInfo.LoTrack = 1;
  DiskInfo.HiTrack = (uint8_t)numTracks();
  DiskInfo.LeadOut = 0;
}

/************************************************************************
*																		*
* 								GetTrackInfo							*
*																		*
************************************************************************/

void GetTrackInfo(char Track) {
}

void GetAudioAddr(char Track) {
}

/************************************************************************
*																		*
*										 Reset							*
*																		*
************************************************************************/

void Reset() {
  //mciSendString("stop cdaudio", NULL, NULL, NULL);
}

int32_t GetCurrTrackNo() {
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

uint32_t Red2HSG(uint32_t RedValue) {
  return 0;
}

uint32_t Red2Secs(uint32_t RedValue) {
  return 0;
}

/************************************************************************
*																		*
*								BCDToBin								*
*																		*
************************************************************************/

uint8_t BCDToBin16(uint8_t BCD) {
  return (0);
}
uint32_t RedBookToSectors(uint8_t RBFrame, uint8_t RBSec, uint8_t RBMin) {
  return (0);
}

/************************************************************************
*																		*
* 									CDPaused							*
*																		*
************************************************************************/

char CDPaused()
// Returns 1 if DA paused, 0 otherwise
{
  if (dosound) {
    mci_status_parms.dwItem = MCI_STATUS_MODE;
    if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
                                (DWORD)(LPVOID)&mci_status_parms)) {
      //mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
      return (0);
    }
    if (mci_status_parms.dwReturn == MCI_MODE_PAUSE)
      return ('1');
    else
      return ('0');
  }
  return ('0');
}

/************************************************************************
*																		*
* 									CDResume							*
*																		*
************************************************************************/

void CDResume() {
  if (dosound) {
    if (retval = mciSendCommand(wDeviceID, MCI_RESUME, NULL, NULL)) {
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

void CDPause() {
  if (dosound) {
    if (retval = mciSendCommand(wDeviceID, MCI_PAUSE, NULL, NULL)) {
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

void CDStop() {
  if (dosound) {
    if (retval = mciSendCommand(wDeviceID, MCI_STOP, NULL, NULL)) {
      //mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
    }
  }
}

void CDRestart() {
  if (dosound) {
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

void CDPlay(int16_t int32_t CurrTrackNo) {
  MCI_PLAY_PARMS mci_play_parms;

  if (dosound) {
    if (CurrTrackNo == 1)
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
                                (DWORD)(LPVOID)&mci_play_parms)) {
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

void CDLoopCheck() {
  if (dosound) {
    mci_status_parms.dwItem = MCI_STATUS_MODE;
    if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
                                (DWORD)(LPVOID)&mci_status_parms)) {
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
*  PARAMETERS: uint8_t Cmd, uint8_t *parm1,					*
*              uint8_t *parm2										*
*																		*
*  DESCRIPTION: Master control for CD functions							*
*																		*
*  RETURNS: 1 if successful												*
*																		*
************************************************************************/

int32_t CdControl(uint8_t Cmd, uint8_t* parm1, uint8_t* parm2) {
  if (dosound) {
    switch (Cmd) {
    case (CdlPlay): {
      if (CDPaused() == 1) {
        CDResume();
      } else {
        CDPlay(CurrTrkNo);
      }
      break;
    }
    case (CdlPause): {
      CDPause();
    } break;
    case (CdlStop): {
      CDStop();
    } break;
    case (CdlSetloc): {
    } break;
    case (CdlSeekL): {
    } break;
    case (CdlSeekP): {
    } break;
    }
  }
  return (1);
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
int32_t numTracks() {
  int32_t num;

  num = CdGetToc2(0, (struct CdlPos*)NULL);
  return num;
}

int32_t CdGetToc2(int32_t sess, struct CdlPos* table) {
  if (dosound) {
    mci_status_parms.dwItem = MCI_STATUS_NUMBER_OF_TRACKS;
    if (retval = mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM,
                                (DWORD)(LPVOID)&mci_status_parms)) {
      mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
      return (0);
    }

    if (mci_status_parms.dwReturn < 2) {
      dosound = FALSE; //cd in but no tracks
      return 2;
    }

    return (mci_status_parms.dwReturn);
  }
  return 0;
}

void CdSetVol(int32_t newVol) {
  UINT ret;
  int32_t theVol;

  return;
  newVol = newVol * (32000 / 218);
  theVol = (newVol << 16) + newVol;

  //ret = auxSetVolume(wDeviceID, theVol);
  ret = auxSetVolume(auxDeviceID, theVol);
}
