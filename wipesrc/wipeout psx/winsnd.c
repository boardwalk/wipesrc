#define DBG 1

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include "utypes.h"
#include "sfx.h"
#define NAME "DSOund Test"
#define TITLE "Piss Flaps"

#define SHIELDS 	15
#define START_SIREN 	16
#define TRACTOR_BEAM 	17

#define NO_SOUND 1
extern int16 error;
#define	WAVOFFSET	0x38
LPDIRECTSOUND 		ppDS;
DSBUFFERDESC		DSBufferParams;
LPDIRECTSOUNDBUFFER	lpPrimarySoundBuffer;
LPDIRECTSOUNDBUFFER	mysample1, samp2, samp3, samp1, samp4;
PCMWAVEFORMAT		format;
LPDIRECTSOUNDBUFFER	*DSSampleArray;
DWORD				*DSFlagsArray;
LPDIRECTSOUNDBUFFER *LoopingArray;
DWORD				*LoopingFlags;
uint16				DSMaxFX;

extern short globalvol;

typedef struct _fEntry
{
	char	filename[256];
	DWORD	flags;
	long	priority;
	int		id;
	struct _fEntry	*next;
} FENTRY;

typedef struct _fPlaying
{
	LPDIRECTSOUNDBUFFER	PlayID;
	int					id;
	struct	_fPlaying	*next;
}	FPLAYING;

char	Sfx8Bit22kDir[255];
char	Sfx16Bit22kDir[255];
char	Sfx8Bit11kDir[255];
char	Sfx16Bit11kDir[255];

int		CurrentID, LastStartID;
FENTRY fEntryRoot, *currentFEntry, *StartFEntry;
FENTRY	**SortedArray;
FPLAYING	*fPlayingList, fPlayingRoot;

int	SfxCreatedAlready = 0;

FENTRY *MakeNewEntry(char *ScriptLine);
int Compare(FENTRY **entry1, FENTRY **entry2);
LPDIRECTSOUNDBUFFER ClonePlayBuf(LPDIRECTSOUNDBUFFER buf, int id);
int WaveLoadFile(char *,UINT *,DWORD *,WAVEFORMATEX **, BYTE **);
void SortFEntries(FENTRY *base);

char szMsg[512] = "";
int	keyhit = 0;
void finiSndObjects( void )
{
	int loop;

    if( ppDS != NULL )
    {
        if( lpPrimarySoundBuffer != NULL )
        {
            lpPrimarySoundBuffer->lpVtbl->Release(lpPrimarySoundBuffer);
            lpPrimarySoundBuffer = NULL;
        }
		
		for (loop =0;loop < CurrentID ;loop++ )
		  {
		  if (DSSampleArray[loop] != NULL)
		    {
		    DSSampleArray[loop]->lpVtbl->Release(DSSampleArray[loop]);
		    }

		  }
        ppDS->lpVtbl->Release(ppDS);
        ppDS = NULL;
    }
} /* finiSndObjects */

void PlaySamp(LPDIRECTSOUNDBUFFER buf, DWORD dwFlags, int id)
{
	HRESULT dsrval;
	DWORD	status;

	buf->lpVtbl->GetStatus(buf, &status);

	if (status & DSBSTATUS_BUFFERLOST)
		buf->lpVtbl->Restore(buf);

	if (status & DSBSTATUS_PLAYING)
		buf = ClonePlayBuf(buf, id);

	dsrval = buf->lpVtbl->Play(buf,0 ,0 ,dwFlags);
	if (dsrval != DS_OK)
	  {
	  if (SoundErr("PlaySamp->Play:", dsrval ))
	    {
		dsrval = buf->lpVtbl->Restore(buf);
		dsrval = buf->lpVtbl->Play(buf,0 ,0 ,dwFlags);
		if (dsrval != DS_OK)
			SoundErr("Buffer restored PlaySamp->Play:", dsrval);
	    }
	  }
}

LPDIRECTSOUNDBUFFER ClonePlayBuf(LPDIRECTSOUNDBUFFER buf, int id)
{
	LPDIRECTSOUNDBUFFER	newbuf;
	FPLAYING	*element;
	DWORD	status;

	element = &fPlayingRoot;

	while (element->next != NULL)
	  {
#if 0
	  	if(element->id == id)
	  	{
	  		newbuf = element->PlayID;
			newbuf->lpVtbl->GetStatus(newbuf, &status);
			if (!(status & DSBSTATUS_PLAYING))
				return(newbuf);
	  	}
#endif
		element = element->next;
	  }

	element->next = (FPLAYING *)malloc(sizeof(FPLAYING));

	element = element->next;
	element->id = id;
	ppDS->lpVtbl->DuplicateSoundBuffer(ppDS, buf, &newbuf);
	element->PlayID = newbuf;
	element->next = NULL;
	return(newbuf);
}

void StopSamp(LPDIRECTSOUNDBUFFER buf)
{
	HRESULT dsrval;
			dsrval = buf->lpVtbl->Stop(buf);
			if (dsrval != DS_OK)
			  {
			  if (SoundErr("StopSamp->Stop:", dsrval ))
			    {
				dsrval = buf->lpVtbl->Restore(buf);
				dsrval = buf->lpVtbl->Stop(buf);
				if (dsrval != DS_OK)
					SoundErr("Buffer restored StopSamp->Stop:", dsrval);
			    }
			  }
}

HWND	MainWin;
BOOL doSndInit(uint16 chans, uint16 sRate, HWND hwnd, uint16 nBits)
{
    HRESULT             dsrval;
	DSCAPS				SoundCaps;

	
	MainWin = hwnd;
	dsrval = DirectSoundCreate(NULL, //default device
								&ppDS,
								NULL);

	if (dsrval != DS_OK)
		{
		SoundErr("DirectSoundCreate:", dsrval );
		return(FALSE);
		}

	dsrval = ppDS->lpVtbl->SetCooperativeLevel(ppDS, hwnd, DSSCL_PRIORITY);
	if (dsrval != DS_OK)
	  {
		SoundErr("SetCooperativeLevel:", dsrval );
		return(FALSE);
	  }
	memset(&DSBufferParams,0,sizeof(DSBUFFERDESC));
	DSBufferParams.dwSize = sizeof(DSBufferParams);
	DSBufferParams.dwFlags = DSBCAPS_PRIMARYBUFFER;
	DSBufferParams.dwBufferBytes = 0; //Must be 0 for primary buffer
	DSBufferParams.lpwfxFormat = NULL; //Must be null for Primary Buffers

	dsrval = ppDS->lpVtbl->CreateSoundBuffer(ppDS, &DSBufferParams, &lpPrimarySoundBuffer, NULL);

	if (dsrval != DS_OK)
	  {
		SoundErr("Create Primary Failed:", dsrval );
		return(FALSE);
	  }
	

	SoundCaps.dwSize = sizeof(SoundCaps);
	dsrval = ppDS->lpVtbl->GetCaps(ppDS, &SoundCaps);

	if (dsrval != DS_OK)
		{
		SoundErr("ppDS->GetCaps:", dsrval );
		}
	else
	  {
	  //make sure sound card can deliver what we've asked it to.  If it can't then try and find an alternative.

	  //Firstly the number of bits

	  //Asked for 16 and can deliver
	  if ((SoundCaps.dwFlags & DSCAPS_PRIMARY16BIT) && (nBits >= 16))
		nBits = 16;
	  //Asked for 16 can't do, but can we do 8
	  else if (!(SoundCaps.dwFlags & DSCAPS_PRIMARY16BIT) && (nBits >= 16) && (SoundCaps.dwFlags & DSCAPS_PRIMARY8BIT))
	    {
	    nBits = 8;
	    }
	  //Asked for 8 can't do but can do 16
	  else if ((nBits == 8) && !(SoundCaps.dwFlags & DSCAPS_PRIMARY8BIT) && (SoundCaps.dwFlags & DSCAPS_PRIMARY16BIT))
		nBits = 16;
	  //Asked for 8 & can do
	  else if ((SoundCaps.dwFlags & DSCAPS_PRIMARY16BIT) && (nBits <= 8))
		nBits = 8;
	  else
	    {
	    return(FALSE);
	    }

	  //Can we verify the min max rate?
	  if (SoundCaps.dwFlags & DSCAPS_CONTINUOUSRATE)
	    {
	    if (sRate > SoundCaps.dwMaxSecondarySampleRate)
			sRate = SoundCaps.dwMaxSecondarySampleRate;
		if (sRate < SoundCaps.dwMinSecondarySampleRate)
			sRate = SoundCaps.dwMinSecondarySampleRate;
	    }

	  //Ask stereo can get stereo
	  if ((SoundCaps.dwFlags & DSCAPS_PRIMARYSTEREO) && (chans == 2))
		chans = 2;
	  // Ask for Stereo can't get can get mono
	  else if (!(SoundCaps.dwFlags & DSCAPS_PRIMARYSTEREO) && (SoundCaps.dwFlags & DSCAPS_PRIMARYMONO) && (chans == 2))
		chans = 1;
	  //Ask for mono can get
	  else if ((SoundCaps.dwFlags & DSCAPS_PRIMARYMONO) && (chans == 1))
		chans = 1;
	  //Ask for mono can't get, can get stereo
	  else if ((SoundCaps.dwFlags & DSCAPS_PRIMARYSTEREO) && !(SoundCaps.dwFlags & DSCAPS_PRIMARYMONO) && (chans == 1))
		chans = 2;
	  else
	    {
	    return(FALSE);
	    }
	  }




	memset(&format,0,sizeof(format));
	format.wf.wFormatTag = WAVE_FORMAT_PCM;
	format.wf.nChannels = chans;
	format.wf.nSamplesPerSec = sRate;
	format.wBitsPerSample = nBits;
	format.wf.nBlockAlign = (format.wf.nChannels * format.wBitsPerSample) / 8;
	format.wf.nAvgBytesPerSec = format.wf.nSamplesPerSec * format.wf.nBlockAlign;

	dsrval = lpPrimarySoundBuffer->lpVtbl->SetFormat(lpPrimarySoundBuffer, (WAVEFORMATEX *)&format);

	if (dsrval != DS_OK)
	  {
		if (SoundErr("Primary->SetFormat:", dsrval ))
		  {
			dsrval = lpPrimarySoundBuffer->lpVtbl->Restore(lpPrimarySoundBuffer);
			dsrval = lpPrimarySoundBuffer->lpVtbl->SetFormat(lpPrimarySoundBuffer, (WAVEFORMATEX *)&format);
			if (dsrval != DS_OK)
				SoundErr("Buffer restored, Primary->SetFormat:", dsrval);
		  }
		return(FALSE);
	  }


	return(TRUE);
		
}

LPDIRECTSOUNDBUFFER GetLoadSecondaryBuffer(char *filename,DWORD dwFlags)
{
	LPDIRECTSOUNDBUFFER	buffer;
	HRESULT		dsrval;
	LPVOID	ptr1, ptr2;
	DWORD	len1, len2;
#ifdef BODGEIT
	FILE *file;
	int length;

	file = fopen(filename,"rb");
	if (file == NULL)
		return(NULL);

	fseek(file, 0,SEEK_END);
	length = ftell(file);
	length -= WAVOFFSET;
	fseek(file,WAVOFFSET,SEEK_SET);
#else
	UINT	length;
	DWORD	noSamp;
	WAVEFORMATEX	*format;
	BYTE	*dataPtr;


	WaveLoadFile(filename, &length, &noSamp, &format, &dataPtr);
#endif


	memset(&DSBufferParams,0,sizeof(DSBUFFERDESC));
	DSBufferParams.dwSize = sizeof(DSBufferParams);
	DSBufferParams.dwFlags = dwFlags;
	DSBufferParams.dwBufferBytes = length; 


	DSBufferParams.lpwfxFormat = format;

	dsrval = ppDS->lpVtbl->CreateSoundBuffer(ppDS, &DSBufferParams, &buffer, NULL);


	if (dsrval != DS_OK)
	  {
		SoundErr("LoadSecondary->Create:", dsrval );
		return(NULL);
	  }

	dsrval = buffer->lpVtbl->Lock(buffer, 0, length, &ptr1, &len1, &ptr2, &len2, 0);

#if 0	//This is very aggresive
	if (len1 != length)
	  {
		fclose(file);
		buffer->lpVtbl->Unlock(buffer, ptr1, len1, ptr2, len2);
	  }
#endif

	if (dsrval != DS_OK)
	  {
	  if (SoundErr("LoadSecondary->Lock:", dsrval ))
	    {
	  	dsrval = buffer->lpVtbl->Restore(buffer);
		dsrval = buffer->lpVtbl->Lock(buffer, 0, length, &ptr1, &len1, &ptr2, &len2, 0);
	  	if (dsrval != DS_OK)
	  		SoundErr("Buffer restored, LoadSecondary->Lock:", dsrval);
	    }
	  return(NULL);
	  }

#ifdef BODGEIT
	fread(ptr1,1,length, file);
	fclose(file);
#else
	memcpy(ptr1, dataPtr, length);
	GlobalFree(dataPtr);
	GlobalFree(format);
#endif
	buffer->lpVtbl->Unlock(buffer, ptr1, len1, ptr2, len2);

	return(buffer);
		
}

LPDIRECTSOUNDBUFFER GetCopySecondaryBuffer(void *buf,long length, DWORD dwFlags)
{
	LPDIRECTSOUNDBUFFER	buffer;
	HRESULT		dsrval;
	LPVOID	ptr1, ptr2;
	DWORD	len1, len2;


	memset(&DSBufferParams,0,sizeof(DSBUFFERDESC));
	DSBufferParams.dwSize = sizeof(DSBufferParams);
	DSBufferParams.dwFlags = dwFlags;
	DSBufferParams.dwBufferBytes = length; 



	format.wf.nChannels = 1;
	format.wf.nBlockAlign = (format.wf.nChannels * format.wBitsPerSample) / 8;
	format.wf.nAvgBytesPerSec = format.wf.nSamplesPerSec * format.wf.nBlockAlign;
	DSBufferParams.lpwfxFormat = (WAVEFORMATEX *)&format;


	dsrval = ppDS->lpVtbl->CreateSoundBuffer(ppDS, &DSBufferParams, &buffer, NULL);


	if (dsrval != DS_OK)
	  {
		SoundErr("CopySecondary->Create:", dsrval );
		return(NULL);
	  }

	dsrval = buffer->lpVtbl->Lock(buffer, 0, length, &ptr1, &len1, &ptr2, &len2, 0);

#if 0	//This is very aggresive
	if (len1 != length)
	  {
		fclose(file);
		buffer->lpVtbl->Unlock(buffer, ptr1, len1, ptr2, len2);
	  }
#endif

	if (dsrval != DS_OK)
	  {
	   if (SoundErr("CopySecondary->Lock:", dsrval ))
	     {
	   		dsrval = buffer->lpVtbl->Restore(buffer);
			dsrval = buffer->lpVtbl->Lock(buffer, 0, length, &ptr1, &len1, &ptr2, &len2, 0);
	   		if (dsrval != DS_OK)
	   			SoundErr("Buffer restored, CopySecondary->Lock:", dsrval);
	     }
	  return(NULL);
	  }

//Copy goes here
	memcpy(ptr1, buffer, length);
	buffer->lpVtbl->Unlock(buffer, ptr1, len1, ptr2, len2);

	return(buffer);
		
}



uint16 SfxInit(uint16 wCard,uint16 wMixType,uint16 wSampRate, void *hwnd)
{
    if( !doSndInit( wMixType, wSampRate , hwnd, 16) )
    {
        finiSndObjects();
		return(1);		//Returns 1 for failure
    }

	fEntryRoot.next = NULL;
	currentFEntry = &fEntryRoot;
	CurrentID = 0;
	DSSampleArray = NULL;
	DSFlagsArray = NULL;
	fPlayingRoot.next  = NULL;
	fPlayingList = &fPlayingRoot;

	return(0);
}

uint16 SfxGetFxBank(char *filename)
{
	char Line[256];
	char 	Scrap[256], Temp[256];
	FILE *file;

	LastStartID = CurrentID;
	StartFEntry = currentFEntry;
	file = fopen(filename, "r");	//ascii file
	if (file == NULL)
	  {
	  return(1);				//file not found
	  }

	Sfx8Bit22kDir[0] = 0;		//All directorys curent unless overriden
	Sfx16Bit22kDir[0] = 0;
	Sfx8Bit11kDir[0] = 0;
	Sfx16Bit11kDir[0] = 0;

	while (fgets(Line, 255, file) != NULL)
	  {
	  if (!strncmp(Line,"8BIT22K", 7))
	    {
	    sscanf(Line,"%s %s",Scrap, Sfx8Bit22kDir);
	    }
	  else if (!strncmp(Line, "8BIT11K", 7))
	    {
	    sscanf(Line,"%s %s",Scrap, Sfx8Bit11kDir);
	    }
	  else if (!strncmp(Line, "16BIT22K", 7))
	    {
	    sscanf(Line,"%s %s",Scrap, Sfx16Bit22kDir);
	    }
	  else if (!strncmp(Line, "16BIT11K", 7))
	    {
	    sscanf(Line,"%s %s",Scrap, Sfx16Bit11kDir);
	    }
	  else if (!strncmp(Line,"FILE", 4))
	    {
		currentFEntry = MakeNewEntry(Line);
//CRAPPER		  currentFEntry->flags |= DSBCAPS_STATIC;  // JJS 3105
	    }
	  else if (!strncmp(Line,"ATTRIB", 6))
	    {
		if (currentFEntry == NULL)
			continue;
	    sscanf(Line, "%s %s",Scrap, Temp);
		if (!strncmp(Line,"SRAM",4))
		  {
		  currentFEntry->flags |= DSBCAPS_STATIC;
		  }
		else if (!strncmp(Line,"PAN",3))
		  {
		  currentFEntry->flags |= DSBCAPS_CTRLPAN;
		  }
		else if (!strncmp(Line,"VOL",3))
		  {
		  currentFEntry->flags |= DSBCAPS_CTRLVOLUME;
		  }
		else if (!strncmp(Line,"PITCH",5))
		  {
		  currentFEntry->flags |= DSBCAPS_CTRLFREQUENCY;
		  }
	    } /*endif ATTRIB*/
	  else if (!strncmp(Line,"PRIORITY", 8))
	    {
		if (currentFEntry != NULL)
		    sscanf(Line,"%s %ld",Scrap,&currentFEntry->priority);
	    }
	  }	/*end while*/
	  if (StartFEntry->next != NULL)	//We've added some
		StartFEntry = StartFEntry->next;
	  return(0);
}

FENTRY *MakeNewEntry(char *ScriptLine)
{
	FENTRY *element;
	char	Scrap[256];

	element = &fEntryRoot;

	while (element->next != NULL)
	  {
	  element = element->next;
	  }

	element->next = (FENTRY *)malloc(sizeof(FENTRY));

	element = element->next;
	if (element == NULL)
		return(NULL);

	sscanf(ScriptLine,"%s %s",Scrap,element->filename);
	element->flags = 0;
	element->priority = 32767;
	element->id = CurrentID++;
	element->next = NULL;
	return(element);
}

int16 SfxOpenFxBank(int16 *pwFirstId)
{
	int numFX, loop;
	LPDIRECTSOUNDBUFFER *NewBuffer;
	DWORD			*NewFlags;
	FENTRY		*element;
	char	RealName[256];


	SortFEntries(StartFEntry);
	numFX = (CurrentID - LastStartID);

	NewBuffer = (LPDIRECTSOUNDBUFFER *)malloc(CurrentID * sizeof (LPDIRECTSOUNDBUFFER *));
	if (NewBuffer == NULL)
		return(-1);
	NewFlags = (DWORD *)malloc(CurrentID*sizeof(DWORD));
	if (NewFlags == NULL)
		return(-1);
		

	for (loop = 0; loop < LastStartID;loop++ )
	  {
	  NewBuffer[loop] = DSSampleArray[loop];	//Copy over the existing pointers
	  NewFlags[loop] = DSFlagsArray[loop];
	  }

	free(DSSampleArray);		//Set this one up as current
	free(DSFlagsArray);
	DSSampleArray = NewBuffer;
	DSFlagsArray = NewFlags;

	for (loop = 0; loop < numFX; loop++)
	  {
	  element = SortedArray[loop];
	  sprintf(RealName,"%s%s",Sfx16Bit22kDir, element->filename);
	  DSSampleArray[element->id] = GetLoadSecondaryBuffer(RealName, element->flags); //Initialise array
	  DSFlagsArray[element->id] = element->flags;
	  }

	*pwFirstId = LastStartID;
	free(SortedArray);
	return(0);
}

void SortFEntries(FENTRY *base)
{
	FENTRY *element;
	int		loop;

	SortedArray = (FENTRY **)malloc((CurrentID - LastStartID)*sizeof(FENTRY *));
	element = base;
	for (loop = 0; loop < (CurrentID - LastStartID);loop++ )
	  {
	  SortedArray[loop] = element;	//Initialise to default order
	  element = element->next;
	  if ((element == NULL) && (loop != (CurrentID -1 - LastStartID)))
		return;
	  }
	
	qsort(SortedArray,(CurrentID - LastStartID), sizeof(FENTRY *), Compare);


}

int Compare(FENTRY **entry1, FENTRY **entry2)
{
	return((*entry1)->priority - (*entry2)->priority);
}

uint16 SfxSetMaxEffects(uint16 wNum)
{
	int loop;
	DSMaxFX = wNum;
	LoopingArray = (LPDIRECTSOUNDBUFFER *)malloc(wNum * sizeof(LPDIRECTSOUNDBUFFER));
	LoopingFlags = (DWORD *)malloc(wNum * sizeof(DWORD));
	for (loop =0; loop < wNum; loop++)
	  {
	  LoopingArray[loop] = NULL;	//Nothing playing to begin with
	  LoopingFlags[loop] = 0;
	  }
	return(0);
}

void SfxSetVolume(uint8 bVolume)
{
	long	volume;
	float nvol;

	if(error==NO_SOUND)
		return;
	nvol = ((float)globalvol / 218.0);

	volume = nvol* (((bVolume * 10000)/255)  - 10000);
	lpPrimarySoundBuffer->lpVtbl->SetVolume(lpPrimarySoundBuffer,volume);
}

void SfxOn(void)
{
	return;
}

void SfxOff(void)
{
	return;
}

uint16 SfxInitReverb(uint16 wReverbType,uint16 wDelay)
{
	return(0);
}
void SfxReverbOn(void)
{
	return;
}
void SfxReverbOff(void)
{
	return;
}
void SfxSetReverbDepth(uint8 bDepth)
{
	return;
}
void SfxUnInitReverb(void)
{
	return;
}
void SfxSetCdVolume(uint8 bVolume)
{
	return;
}

void SfxPlay(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags)
{
	long volume;
	long pan;
	DWORD	frequency;
	float nvol;

	nvol = ((float)globalvol / 218.0);

	volume = nvol * (( (bVolume * 10000)/255)  - 10000);
	pan = ((bLRPan-15) * 10000) / 15;
	frequency = (22050 * wPitch) / 256;

	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLVOLUME)
		DSSampleArray[wSampleId]->lpVtbl->SetVolume(DSSampleArray[wSampleId],volume);
	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLPAN)
		DSSampleArray[wSampleId]->lpVtbl->SetPan(DSSampleArray[wSampleId],pan);
	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLFREQUENCY)
		DSSampleArray[wSampleId]->lpVtbl->SetFrequency(DSSampleArray[wSampleId],frequency);

	PlaySamp(DSSampleArray[wSampleId], 0, wSampleId);
}

int16 SfxPlayID(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags)
{
	long volume;
	long pan;
	DWORD	frequency;
	DWORD	flags;
	int loop;
	float nvol;

	nvol = ((float)globalvol / 218.0);

	volume = nvol * (( (bVolume * 10000)/255)  - 10000);

	pan = ((bLRPan-15) * 10000) / 15;
	frequency = (22050 * wPitch) / 256;

	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLVOLUME)
		DSSampleArray[wSampleId]->lpVtbl->SetVolume(DSSampleArray[wSampleId],volume);
	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLPAN)
		DSSampleArray[wSampleId]->lpVtbl->SetPan(DSSampleArray[wSampleId],pan);
	if (DSFlagsArray[wSampleId] & DSBCAPS_CTRLFREQUENCY)
		DSSampleArray[wSampleId]->lpVtbl->SetFrequency(DSSampleArray[wSampleId],frequency);


	for (loop =0;loop< DSMaxFX ; loop++)
	  {
	  	if (LoopingArray[loop] == NULL)
			break;
	  }

	if (loop == DSMaxFX)
		return(-1);	

	LoopingArray[loop] = DSSampleArray[wSampleId];
	LoopingFlags[loop]  = DSFlagsArray[wSampleId];

	if (bFlags & LOOP_ON)
		flags = DSBPLAY_LOOPING;
	else
		flags = 0;

	PlaySamp(DSSampleArray[wSampleId], flags, wSampleId);
	return(loop);
}

void SfxEffectVol(int16 wPlayId,uint8 bVolume)
{
	DWORD	status;
	long volume;
	float nvol;

	nvol = ((float)globalvol / 218.0);
	volume = ( (bVolume * 10000)/255)  - 10000;

	if (LoopingArray[wPlayId] == NULL)
		return;

	LoopingArray[wPlayId]->lpVtbl->GetStatus(LoopingArray[wPlayId],&status);

	switch (status)
	  {
	  case DSBSTATUS_BUFFERLOST:
		LoopingArray[wPlayId]->lpVtbl->Restore(LoopingArray[wPlayId]);
	  	break;
	  case DSBSTATUS_PLAYING:
	  case DSBSTATUS_LOOPING:
	  	break;
	  default:
	  	LoopingArray[wPlayId] = NULL;
		LoopingFlags[wPlayId] = 0;
		return;
	  	break;
	  }

	if (LoopingFlags[wPlayId] & DSBCAPS_CTRLVOLUME)
		LoopingArray[wPlayId]->lpVtbl->SetVolume(LoopingArray[wPlayId],volume);

}

void SfxEffectPitch(int16 wPlayId,uint16 wPitch)
{
	DWORD	status;
	DWORD	frequency;

	frequency = (22050 * wPitch) / 256;

	if (LoopingArray[wPlayId] == NULL)
		return;

	LoopingArray[wPlayId]->lpVtbl->GetStatus(LoopingArray[wPlayId],&status);

	switch (status)
	  {
	  case DSBSTATUS_BUFFERLOST:
		LoopingArray[wPlayId]->lpVtbl->Restore(LoopingArray[wPlayId]);
	  	break;
	  case DSBSTATUS_PLAYING:
	  case DSBSTATUS_LOOPING:
	  	break;
	  default:
	  	LoopingArray[wPlayId] = NULL;
		LoopingFlags[wPlayId] = 0;
		return;
	  	break;
	  }

	if (LoopingFlags[wPlayId] & DSBCAPS_CTRLFREQUENCY)
		LoopingArray[wPlayId]->lpVtbl->SetVolume(LoopingArray[wPlayId],frequency);

}



void SfxEffectPan(int16 wPlayId,uint16 bPan)
{
	DWORD	status;
	long pan;

	pan = ((bPan-15) * 10000) / 15;

	if (LoopingArray[wPlayId] == NULL)
		return;

	LoopingArray[wPlayId]->lpVtbl->GetStatus(LoopingArray[wPlayId],&status);

	switch (status)
	  {
	  case DSBSTATUS_BUFFERLOST:
		LoopingArray[wPlayId]->lpVtbl->Restore(LoopingArray[wPlayId]);
	  	break;
	  case DSBSTATUS_PLAYING:
	  case DSBSTATUS_LOOPING:
	  	break;
	  default:
	  	LoopingArray[wPlayId] = NULL;
		LoopingFlags[wPlayId] = 0;
		return;
	  	break;
	  }

	if (LoopingFlags[wPlayId] & DSBCAPS_CTRLPAN)
		LoopingArray[wPlayId]->lpVtbl->SetVolume(LoopingArray[wPlayId],pan);

}

void SfxUnInit(uint8 bFlag)
{
	finiSndObjects();
}

void SfxStop(int16 wPlayId)
{
	DWORD status;

	if (LoopingArray[wPlayId] == NULL)
		return;

	LoopingArray[wPlayId]->lpVtbl->GetStatus(LoopingArray[wPlayId],&status);

	if (status & DSBSTATUS_BUFFERLOST)
		LoopingArray[wPlayId]->lpVtbl->Restore(LoopingArray[wPlayId]);

	if (!status)
	  {
	  LoopingArray[wPlayId] = NULL;
	  LoopingFlags[wPlayId] = 0;
	  return;
	  }

	LoopingArray[wPlayId]->lpVtbl->Stop(LoopingArray[wPlayId]);
	LoopingArray[wPlayId]->lpVtbl->SetCurrentPosition(LoopingArray[wPlayId], 0);	//Reset to beginning
	LoopingArray[wPlayId] = NULL;
	LoopingFlags[wPlayId] = 0;
}

void KillAllFx(void)
{
#if 0
	int loop;

	for (loop =0;loop< CurrentID ; loop++)
	  {
		  if (DSSampleArray[loop] != NULL)
		    {
		    DSSampleArray[loop]->lpVtbl->Stop(DSSampleArray[loop]);
		    }
	  }
#else
	KillNote(SHIELDS);	// just kill the shields
	KillNote(START_SIREN);	// just kill the shields
	KillNote(TRACTOR_BEAM );	// just kill the shields
#endif
}

void SfxDoSfx(void)
{
	FPLAYING *element, *del;
	DWORD	status;

	element = &fPlayingRoot;

	while (element->next != NULL)
	  {
	  element->next->PlayID->lpVtbl->GetStatus(element->next->PlayID, &status);
	  if (!(status & DSBSTATUS_PLAYING))	//Not playing
	    {
	    del = element->next;
		element->next = element->next->next;
		free(del);
	    }
	  if (element->next != NULL)	//Case of deleting the last one
		  element = element->next;
	  }
}

int16 SfxRegisterBuffer(void *dataptr, uint32 length, int16 *idno, long flags)
{
	LPDIRECTSOUNDBUFFER *NewBuffer;
	DWORD			*NewFlags;
	int loop;

	NewBuffer = (LPDIRECTSOUNDBUFFER *)malloc(CurrentID * sizeof (LPDIRECTSOUNDBUFFER *));
	if (NewBuffer == NULL)
		return(1);
	NewFlags = (DWORD *)malloc(CurrentID*sizeof(DWORD));
	if (NewFlags == NULL)
		return(1);

	for (loop = 0; loop < LastStartID;loop++ )
	  {
	  NewBuffer[loop] = DSSampleArray[loop];	//Copy over the existing pointers
	  NewFlags[loop] = DSFlagsArray[loop];
	  }
	free(DSSampleArray);		//Set this one up as current
	free(DSFlagsArray);
	DSSampleArray = NewBuffer;
	DSFlagsArray = NewFlags;

	DSSampleArray[CurrentID] = GetCopySecondaryBuffer(dataptr, length, flags);
	DSFlagsArray[CurrentID] = flags;

	if (DSSampleArray[CurrentID] == NULL)
		return(1);


	LastStartID = CurrentID;
	*idno = CurrentID;
	CurrentID++;

	return(0);
	
}


int	SoundErr(char *String, HRESULT errVal)
{
#if 0
char	buffer[512];
char	title[] = "SOUND ERROR";

switch (errVal)
  {
  case DSERR_ALLOCATED:
  	sprintf(szMsg,"Resources already in use\n");
  	break;
  case DSERR_CONTROLUNAVAIL:
  	sprintf(szMsg,"Requested controller is not availible\n");
  	break;
  case DSERR_INVALIDPARAM:
  	sprintf(szMsg,"Got a parameter wrong fool\n");
  	break;
  case DSERR_INVALIDCALL:
  	sprintf(szMsg,"Your call was wrong\n");
  	break;
  case DSERR_GENERIC:
  	sprintf(szMsg,"Undocumented error\n");
  	break;
  case DSERR_PRIOLEVELNEEDED:
  	sprintf(szMsg,"Insufficient priority for request\n");
  	break;
  case DSERR_NOAGGREGATION:
  	sprintf(szMsg,"The object does not support aggregation\n");
  	break;
  case DSERR_NODRIVER:
  	sprintf(szMsg,"You don't have a sound card\n");
  	break;
  case DSERR_UNSUPPORTED:
  	sprintf(szMsg,"Function called is not supported on this device\n");
  	break;
  case DSERR_OUTOFMEMORY:
  	sprintf(szMsg,"Out of memory for sound buffers\n");
  	break;
  case DSERR_BADFORMAT:
  	sprintf(szMsg,"Bad wave format !!!???!!!\n");
  	break;
  case DSERR_BUFFERLOST:
	return(1);		//RESTORE BUFFER
  	break;
  default:
  	sprintf(szMsg,"Unknown error message\n");
  	break;
  }
 sprintf(buffer,"%s: %s", String, szMsg);
// MessageBox(MainWin, buffer, title, MB_APPLMODAL | MB_ICONHAND | MB_OK);
#endif
 return(0);

}



