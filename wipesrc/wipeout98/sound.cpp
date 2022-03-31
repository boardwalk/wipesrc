#define _SIZE_T_DEFINED_
#include <stdio.h>
#include <string.h>
//#include <libspu.h>
//#include <libsnd.h>


#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "menus.h"
#include "global.h"

#include "utypes.h"
#include "sfx.h"
#include "winsnd.h"
#include "utils.h"
#include "libgte.h"

#define NO_SOUND	1

#define	MAX_FX		30

#define PLAY_CHANNELS	8

uint8_t ConvVol(int16_t left, int16_t right);
uint8_t ConvPan(int16_t left, int16_t right);
uint8_t ConvCDVol(int16_t LeftVol, int16_t RightVol);
void SetVolumeFade(int16_t vagID, int16_t vol);

SFX_INFO	channel_info[PLAY_CHANNELS]; //Information on the channels currently being played

int16_t				sfx_handle[MAX_FX]; // PC sound
int16_t				error;				// PC sound
SampRec				SampTable[MAX_FX];	// PC Sound

static int8_t	bOrg,bImg;							   //PC File handling gubbins
static uint8_t		*pHeadPtr, *pOrgPtr,*pImgPtr;
static FILE 		*pHeadFile,*pOrgFile,*pImgFile;
static char 		pbFileName[80];
static uint8_t		bIndx;
static int16_t		wFirstId;


/*
** WhatChannel
*
*  FILENAME: c:\source\wipeout5\origsrc\sound.c
*
*  PARAMETERS:	vagNo
*
*  DESCRIPTION:
*
*  RETURNS:	the channel number of the VAG if its playing
*				otherwise -1
*/

int16_t WhatChannel(int16_t vagNo)
{
	int32_t i;

	for(i = 0; i < PLAY_CHANNELS; i++)
	{
		if (vagNo == channel_info[i].vagNo)  //is our VAG playing
		{
			return(i);
		}
	}
	return(-1);
}

/*
** IsPlaying
*
*  FILENAME: c:\source\wipeout5\origsrc\sound.c
*
*  PARAMETERS:
*
*  DESCRIPTION:
*
*  RETURNS:   	1 if channel is availble for modify
*				0 otherwise
*/

int32_t IsPlaying(int16_t channel)
{
	if ((channel < 0) || (channel >=PLAY_CHANNELS))
		return(0);		//Revalidate the channel number

	if (channel_info[channel].PlayID >= 0) //looping sound
		return(1);
	else
		return(0);
}



/*
** ConVol
*
*  FILENAME: c:\source\wipeout2\origsrc\sound.c
*
*  PARAMETERS:	shorts left & right volume
*
*  DESCRIPTION: Converts spu vol to PC val
*
*  RETURNS: uint 8 PCVol
*
*/

uint8_t ConvVol(int16_t left, int16_t right)
{
	int32_t Sum;
	uint8_t PCVol;

	Sum = left+right;
	Sum /= 2;
	PCVol = Sum >>6;
	return(PCVol);
}

/*
** ConvPan
*
*  FILENAME: c:\source\wipeout2\origsrc\sound.c
*
*  PARAMETERS:	shorts left & right vol
*
*  DESCRIPTION: Gets PC pan val
*
*  RETURNS: unint8 PCPan
*
*/

uint8_t ConvPan(int16_t left, int16_t right)
{
	float ratio;
	uint8_t PCPan;

	if (left > 0 && right > 0)
	{

		ratio = (float)left/right;

		if (ratio ==1)
		{
			PCPan=15;
		}
		else
			if (ratio >1)
			{
				PCPan = (uint8_t)((uint16_t) ratio >>13);
			}
			else
			{
				PCPan = (uint8_t)((uint16_t) ratio >>13)+15;
			}

			if (PCPan>30)
				PCPan =30;

			return(PCPan);
	} else
		return 0;
}

/*
** ConvPitch
*
*  FILENAME: c:\source\wipeout2\origsrc\sound.c
*
*  PARAMETERS:	uint16_t Pitch
*
*  DESCRIPTION: converts spu pitch to PC pitch
*
*  RETURNS: uint16_t PCPitch;
*
*/

uint16_t ConvPitch(uint16_t Pitch)
{
	uint16_t PCPitch;

	if (Pitch < 0x800)
		Pitch=0x800;

	if (Pitch >0x3fff)
		Pitch =0x3fff;

	if (Pitch==0x3fff)
		PCPitch=768;

	if ((Pitch>=0x800) && (Pitch<0x1000))
    {
		PCPitch = (((Pitch-0x800)>>1)+128);
	}

	if ((Pitch>=0x1000) && (Pitch<0x2000))
	{
		PCPitch = (((Pitch-0x1000)>>4)+256);
	}

	if ((Pitch>=0x2000) && (Pitch<0x3fff))
	{
		PCPitch = (((PCPitch-0x2000)>>5)+512);
	}
	PCPitch*=2;

	return(PCPitch);
}
/*

  ** ConvCDVol
  *
  *  FILENAME: c:\source\wipeout2\origsrc\sound.c
  *
  *  PARAMETERS:
  *
  *  DESCRIPTION:
  *
  *  RETURNS:
  *
*/
uint8_t ConvCDVol(int16_t LeftVol, int16_t RightVol)
{
	int16_t CDVol;
	uint8_t PCVol;

	CDVol=(LeftVol+RightVol)/2;
	PCVol=(uint8_t)(CDVol>>7);

	return(PCVol);
}

/************************************************************/
/***** Initialises the sound sub system, loads VAB file *****/
/*****  & sets channel manager                          *****/
/************************************************************/

void InitSpuSound()
{
	int32_t		i;
	int32_t		decval;

	error = SfxInit(AUTO_DETECT,STEREO,SR_22K);
	if (error !=1)
	{
		/* Load bank header file */


		SfxGetFxBank ("wipeout/wipeout.fx");

		/* Open FX Bank */

		SfxOpenFxBank(&wFirstId);

		/* Build Handles To FX*/

		for(bIndx=0;bIndx<MAX_FX;bIndx++)
		{
			sfx_handle[bIndx] = wFirstId+bIndx;
		}
	}


	// SetUp Sample Info per Sample

	for(i = 0; i < MAX_FX; i++)
	{
		SampTable[i].SampVol =100;		//All Samples MAX volume
		SampTable[i].SampPan =15;		//Centred
		SampTable[i].SampPitch =256;		//Default Pitch

											/*if (i==ENG_AIR_INTAKE ||
											i==ENG_RUMBLE ||
											i==ENG_THRUST ||
											i==REMOTE_ENG ||
											i==SHIELDS ||
											i==TRACTOR_BEAM ||
   	    i==TURBULENCE ||#*/
		if (i==REMOTE_ENG ||
			i==SHIELDS ||
			i==TRACTOR_BEAM ||
			i==TURBULENCE ||
			i==CROWD)
		{
			SampTable[i].Loop =1;
		}
		else
		{
			SampTable[i].Loop=0;
		}

		SampTable[i].Playing =0;
	}

	if (error !=NO_SOUND)
	{
		SfxSetMaxEffects(PLAY_CHANNELS); //Up to eight effects
		SfxSetVolume(100); 				//Set Volume to maximum.
		SfxOn();						//Start to DMA circ buffer.

		/** Init Reverb **/
		SfxInitReverb(REVERB,100);		//Default value for reverb effect
		SfxSetReverbDepth(64);			//ditto
		SfxReverbOff();					//No reverb by refault
	}

	//Initialise the Play channels
	for(i = 0; i < PLAY_CHANNELS; i++)
	{
		channel_info[i].Volume = 0;
		channel_info[i].Pan    = 15;
		channel_info[i].Pitch  = 256;
		channel_info[i].vagNo  = -1;
		channel_info[i].Playing = 0;
	}

	//THIS ONE IS STILL INTERESTING
	chanTable[POWER_UP].s_attr.pitch = 0x0800;

	sampleLevel = 8190;

	//Set the DA volume to that required
	SfxSetCdVolume(ConvCDVol(c_attr.cd.volume.left,c_attr.cd.volume.right));
}


void UpdateNotes(int32_t paused)
{
	int32_t i;
	static int32_t mode = NOT_PAUSED;


	if ((mode == PAUSED) && (paused != PAUSED))
		//coming off pause mode and back into normal mode
		//at the moment has no changes from normal mode
	{
		mode = NOT_PAUSED;
		for (i = 0; i < PLAY_CHANNELS; i++)
		{
			//Static spot effect and fading out loop samples
			if (channel_info[i].Duration > 0)
			{
				if (--channel_info[i].Duration == 0)
				{
					if (channel_info[i].PlayID == -2) //Spot effect
					{
						channel_info[i].PlayID = -1;
						channel_info[i].Volume = 0;
						channel_info[i].Pan = 15;
						channel_info[i].Pitch = 256;
						channel_info[i].vagNo = -1;
						channel_info[i].Playing = 0;
						channel_info[i].Priority = 0x7fff;
					}
					else //Terminated looping effect
					{
						KillNote(channel_info[i].vagNo);
					}
				} //channel still playing
			}
		}//end for
	} //end if mode ==
	else if ((mode == NOT_PAUSED) && (paused == PAUSED))
		//FIRST paused section
		//looping sounds should now switch to duration 8
	{
		mode = PAUSED;
		KillAllFx();
		for (i = 0; i < PLAY_CHANNELS; i++)
		{
			//Static spot effect and fading out loop samples
			if (channel_info[i].Duration > 0)
			{
				if (--channel_info[i].Duration == 0)
				{
					if (channel_info[i].PlayID == -2) //Spot effect
					{
						channel_info[i].PlayID = -1;
						channel_info[i].Volume = 0;
						channel_info[i].Pan = 15;
						channel_info[i].Pitch = 256;
						channel_info[i].vagNo = -1;
						channel_info[i].Playing = 0;
						channel_info[i].Priority = 0x7fff;
					}
					else //Terminated looping effect
					{
						KillNote(channel_info[i].vagNo);
					}
				} //channel still playing
			}
			else if (channel_info[i].Duration == -1)
			{
				channel_info[i].Duration = 8; //8 frames to fade out
				SetVolumeFade(channel_info[i].vagNo, channel_info[i].Volume>>1);
			}
		}//end for
	} //end if mode ==
	else if ((mode == NOT_PAUSED) && (paused == PAUSED))
		//Subsequent paused section
	{
		for (i = 0; i < PLAY_CHANNELS; i++)
		{
			//Static spot effect and fading out loop samples
			if (channel_info[i].Duration > 0)
			{
				if (--channel_info[i].Duration == 0)
				{
					if (channel_info[i].PlayID == -2) //Spot effect
					{
						channel_info[i].PlayID = -1;
						channel_info[i].Volume = 0;
						channel_info[i].Pan = 15;
						channel_info[i].Pitch = 256;
						channel_info[i].vagNo = -1;
						channel_info[i].Playing = 0;
						channel_info[i].Priority = 0x7fff;
					}
					else //Terminated looping effect
					{
						KillNote(channel_info[i].vagNo);
					}
				} //channel still playing
				else if (channel_info[i].PlayID >=0)	//Fade out paused sample
				{
					SetVolumeFade(channel_info[i].vagNo, channel_info[i].Volume>>1);
				}
			}
		}//end for
	} //end if mode ==
	else //if ((mode == NOT_PAUSED) && (paused == NOT_PAUSED))
		//Normal update
	{
		for (i = 0; i < PLAY_CHANNELS; i++)
		{
			//Static spot effect and fading out loop samples
			if (channel_info[i].Duration > 0)
			{
				if (--channel_info[i].Duration == 0)
				{
					if (channel_info[i].PlayID == -2) //Spot effect
					{
						channel_info[i].PlayID = -1;
						channel_info[i].Volume = 0;
						channel_info[i].Pan = 15;
						channel_info[i].Pitch = 256;
						channel_info[i].vagNo = -1;
						channel_info[i].Playing = 0;
						channel_info[i].Priority = 0x7fff;
					}
					else //Terminated looping effect
					{
						KillNote(channel_info[i].vagNo);
					}
				} //channel still playing
			}
		}//end for
	} //end if mode ==




}

void KillNote(int32_t note)
{
	int16_t i;
	int16_t channel;

	if (error == NO_SOUND)
		return;

	channel = WhatChannel(note);

	if (IsPlaying(channel))
	{
		SfxStop(channel_info[channel].PlayID);
		channel_info[channel].Volume = 0;
		channel_info[channel].Pan = 15;
		channel_info[channel].Pitch = 256;
		channel_info[channel].Playing  = 0;
		channel_info[channel].Priority = 0x7fff;
		channel_info[channel].Duration = 0;
		channel_info[channel].PlayID = -1;
		SampTable[note].Playing = 0;
	}
}


void KillNotes()
{
	int32_t i;

	if (error == NO_SOUND)
		return;


	for(i = 0; i < PLAY_CHANNELS; i++)
	{
		if (IsPlaying(i))
			KillNote(channel_info[i].vagNo);
	}
	KillAllFx();
}

void StopNotes(void)
{
	int32_t i;

	for (i = 0; i < PLAY_CHANNELS; i++)
		channel_info[i].Playing = 0;
}


/*
Playnote:

  Attempt to find a free channel from PLAY_CHANNELS;
		If effect note already playing
		Start Effect on that channel
		If failed first on lower priority,
		Stop Lower priority one playing
		Start New effect

*/
int16_t PlayNote(int16_t vagID, int16_t priority, int16_t duration)
{

	uint16_t i;

	if (error == NO_SOUND)
		return(-1);

	for( i = 0; i < PLAY_CHANNELS; i++)
	{
		if(channel_info[i].Playing == 0)		//found a completely empty channel
		{
			if (SampTable[vagID].Playing==0)	//Is our sample already playing?
			{
				if (SampTable[vagID].Loop==0)		//Spot effect
				{
					SfxPlay(sfx_handle[vagID],
						SampTable[vagID].SampVol,
						SampTable[vagID].SampPan,
						15,
						SampTable[vagID].SampPitch,
						ALL_OFF);
					channel_info[i].PlayID = -2;	//Spot effect
				}
				else
				{
					switch (vagID)
					{
						//Ignore the following effects as they are bollox
					case ENG_AIR_INTAKE:
					case ENG_RUMBLE:
					case ENG_THRUST:
					case CROWD:
					case REMOTE_ENG:
					case TURBULENCE:
						return(-1);
						break;
					default:
						break;
					}
					SampTable[vagID].PlayID = SfxPlayID(sfx_handle[vagID],
						SampTable[vagID].SampVol,
						SampTable[vagID].SampPan,
						15,
						SampTable[vagID].SampPitch,
						LOOP_ON);
					channel_info[i].PlayID = SampTable[vagID].PlayID;
					SampTable[vagID].Playing = 1;
				}
				channel_info[i].Volume = SampTable[vagID].SampVol;
				channel_info[i].Pan = SampTable[vagID].SampPan;
				channel_info[i].Pitch = SampTable[vagID].SampPitch;
				channel_info[i].vagNo = vagID;
				channel_info[i].Playing = 1;
				channel_info[i].Duration = duration;
				channel_info[i].Priority = priority;
			}
			return(i);
		}
	}

	// Have not found an empty channel
	// therefore consider priority

	for( i = 0; i < PLAY_CHANNELS; i++)
	{
		if(priority < channel_info[i].Priority)		//found a completely empty channel
		{
			if (SampTable[vagID].Playing==0)	//Is our sample already playing?
			{
				if (SampTable[vagID].Loop==0)		//Spot effect
				{
					SfxPlay(sfx_handle[vagID],
						SampTable[vagID].SampVol,
						SampTable[vagID].SampPan,
						15,
						SampTable[vagID].SampPitch,
						ALL_OFF);
				}
				else
				{
					switch (vagID)
					{
						//				  Ignore the following effects as they are bollox
					case ENG_AIR_INTAKE:
					case ENG_RUMBLE:
					case ENG_THRUST:
					case CROWD:
					case REMOTE_ENG:
					case TURBULENCE:
						return(-1);
						break;
					default:
						break;
					}
					SampTable[vagID].PlayID = SfxPlayID(sfx_handle[vagID],
						SampTable[vagID].SampVol,
						SampTable[vagID].SampPan,
						15,
						SampTable[vagID].SampPitch,
						LOOP_ON);
					SampTable[vagID].Playing = 1;
					channel_info[i].PlayID = SampTable[vagID].PlayID;
				}
				channel_info[i].Volume = SampTable[vagID].SampVol;
				channel_info[i].Pan = SampTable[vagID].SampPan;
				channel_info[i].Pitch = SampTable[vagID].SampPitch;
				channel_info[i].vagNo = vagID;
				channel_info[i].Playing = 1;
				channel_info[i].Duration = duration;
				channel_info[i].Priority = priority;
			}
			return(i);
		}
	}

	return(-1);		//No space for sample
}


int16_t PlayDirectionalNote(int16_t vagID, int16_t priority, int16_t duration, VECTOR pos )
{
	uint16_t 			i;
	VECTOR			distance;
	int32_t				mag;
	int32_t				voll, volr;
	int16_t 			angle;
	int16_t 		channel;
	int16_t			PlayID;


	if (error == NO_SOUND)
		return(-1);

	channel =-1;

	distance.vx = (pos.vx + cameraPtr->camPos->relative.t[0])>>3;
	distance.vy = (pos.vy + cameraPtr->camPos->relative.t[1])>>3;
	distance.vz = (pos.vz + cameraPtr->camPos->relative.t[2])>>3;

	mag = GetMagnitude(distance);

	voll = 0x2fff - (mag);
	if(voll < 0) voll = 0;


	volr = 0x2fff - (mag);
	if(volr < 0) volr = 0;


	angle = -ratan2(distance.vx, distance.vz) + cameraPtr->hdg ;
	angle = ang(angle);

	if(angle > 1024) angle = 1024 - (angle - 1024);
	if(angle < -1024) angle = -1024 - (angle + 1024);

	voll += (voll * angle)>>11;
	volr -= (volr * angle)>>11;
	voll = (voll * sampleLevel)>>15;
	volr = (volr * sampleLevel)>>15;


	//now got the volumes
	if((voll > 0) && (volr > 0))
	{
		channel = PlayNote(vagID, priority, duration);	//channel on

		if (channel!=-1)
        {
			PlayID = channel_info[channel].PlayID;			//PlayID for channel
			if(PlayID >=0)
			{
				channel_info[channel].Volume = ConvVol(voll,volr);
				channel_info[channel].Pan = ConvPan(voll,volr);

				SfxEffectVol(PlayID,channel_info[channel].Volume);
				SfxEffectPan(PlayID,channel_info[channel].Pan);
			}
        }

	}
	return(1);
}

void SetPitch(int16_t VagID, int16_t pitch)
{
	uint16_t 			i;
	int16_t 		channel;

	if (error == NO_SOUND)
		return;
	if(pitch > 0x3fff) pitch = 0x3fff;

	channel = WhatChannel(VagID);

	if (IsPlaying(channel))
	{
		channel_info[channel].Pitch = ConvPitch(pitch);	//update the playing pitch
		SfxEffectPitch(channel_info[channel].PlayID,
			channel_info[channel].Pitch);
	}
}

void SetVolume(int16_t vagID, int16_t voll, int16_t volr)
{
	uint16_t 			i;
	int16_t 		channel;

	if (error == NO_SOUND)
		return;

	if(voll > 0x3fff) voll = 0x3fff;
	if(volr > 0x3fff) volr = 0x3fff;

	channel = WhatChannel(vagID);

	if (IsPlaying(channel))
	{
		channel_info[channel].Volume = ConvVol(voll,volr);
		channel_info[channel].Pan = ConvPan(voll,volr);
		SfxEffectVol(channel_info[channel].PlayID,
			channel_info[channel].Volume);
		SfxEffectPan(channel_info[channel].PlayID,
			channel_info[channel].Pan);

	}
}

void SetVolumeFade(int16_t vagID, int16_t vol)
{
	uint16_t 			i;
	int16_t 		channel;

	if (error == NO_SOUND)
		return;

	channel = WhatChannel(vagID);

	if (IsPlaying(channel))
	{
		channel_info[channel].Volume = vol;
		SfxEffectVol(channel_info[channel].PlayID,
			channel_info[channel].Volume);
	}
}



int16_t AdjustDirectionalNote(int16_t vagID, int16_t priority, int16_t duration, VECTOR pos )
{
	uint16_t 			i;
	VECTOR			distance;
	int32_t				mag;
	int32_t				voll, volr;
	int16_t 			angle;
	int16_t 			channel;


	distance.vx = (pos.vx + cameraPtr->camPos->relative.t[0])>>3;
	distance.vy = (pos.vy + cameraPtr->camPos->relative.t[1])>>3;
	distance.vz = (pos.vz + cameraPtr->camPos->relative.t[2])>>3;

	mag = GetMagnitude(distance);
	mag = mag<<2 ;

	voll = 0x2fff - (mag); /* No Scaling */
	if(voll < 0) voll = 0;

	volr = 0x2fff - (mag); /* No Scaling */
	if(volr < 0) volr = 0;

	angle = -ratan2(distance.vx, distance.vz) + cameraPtr->hdg ;
	angle = ang(angle);

	if(angle > 1024) angle = 1024 - (angle - 1024);
	if(angle < -1024) angle = -1024 - (angle + 1024);

	voll += (voll * angle)>>11;  /* 11 */
	volr -= (volr * angle)>>11;  /* 11 */

	voll = (voll * sampleLevel)>>15;
	volr = (volr * sampleLevel)>>15;

	if((voll > 0) && (volr > 0))
	{
		SetVolume(vagID,voll,volr);
	}
	return(1);
}


int16_t AdjustRemoteEngineSound(ShipData *shipIndex, Object** shipShapes, TrackCamera *camera )
{
	uint16_t 			i;
	VECTOR			distance;
	int32_t				mag , smallestMag[3];
	int16_t				ident[3];
	int32_t				voll, volr;
	int16_t 			angle;
	VECTOR			dopler;
	int32_t				doplerMag;

	return 0;

	smallestMag[0] = 14000;
	smallestMag[1] = 14000;
	smallestMag[2] = 14000;
	ident[0] = -1;
	ident[1] = -1;
	ident[2] = -1;

	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		if((i != ownShip) && ((ownShip & RACING)== 0))
		{
			distance.vx = (shipIndex[i].ppivot.vx + camera->camPos->relative.t[0]) >>3;
			distance.vy = (shipIndex[i].ppivot.vy + camera->camPos->relative.t[1]) >>3;
			distance.vz = (shipIndex[i].ppivot.vz + camera->camPos->relative.t[2]) >>3;

			mag = GetMagnitude(distance);

			if(mag < smallestMag[0])
			{
				if(ident[0] != -1)
				{
					smallestMag[1] = smallestMag[0];
					ident[1] = ident[0];
				}
				smallestMag[0] = mag;
				ident[0] = i;
			}
			else if(mag < smallestMag[1])
			{
				smallestMag[1] = mag;
				ident[1] = i;
			}
		}
	}

	for(i = 0; i < 2; i++)
	{
		if(ident[i] > -1)
		{
			distance.vx = (shipIndex[ident[i]].ppivot.vx + camera->camPos->relative.t[0]) >>3;
			distance.vy = (shipIndex[ident[i]].ppivot.vy + camera->camPos->relative.t[1]) >>3;
			distance.vz = (shipIndex[ident[i]].ppivot.vz + camera->camPos->relative.t[2]) >>3;

			mag = GetMagnitude(distance);

			if(mag > 1000)
				voll = 0x2fff - ((mag-1000)<<2);
			else
				voll = 0x2fff;
			if(voll < 0) voll = 0;

			if(mag > 1000)
				volr = 0x2fff - ((mag-1000)<<2);
			else
				volr = 0x2fff;
			if(volr < 0) volr = 0;

			angle = -ratan2(distance.vx, distance.vz) + camera->hdg ;
			angle = ang(angle);

			if(angle > 1024) angle = 1024 - (angle - 1024);
			if(angle < -1024) angle = -1024 - (angle + 1024);

			voll += (voll * angle)>>11;
			volr -= (volr * angle)>>11;

			voll = (voll * sampleLevel)>>15;
			volr = (volr * sampleLevel)>>15;

			dopler.vx = (shipIndex[ident[i]].vpivot.vx>>6) - camera->vel.vx;
			dopler.vy = (shipIndex[ident[i]].vpivot.vy>>6) - camera->vel.vy;
			dopler.vz = (shipIndex[ident[i]].vpivot.vz>>6) - camera->vel.vz;

			dopler.vx += distance.vx;
			dopler.vy += distance.vy;
			dopler.vz += distance.vz;

			doplerMag = mag - GetMagnitude(dopler) ;

			SetPitch(REMOTE + i, 0x0800 + doplerMag);
			SetVolume(REMOTE + i, voll, volr);

		}
		else
		{
			SetVolume(REMOTE + i, 0, 0);
		}
	}
	return(1);
}

/*********************************************************************/
/**** WiPe OuT specific functions to adjust in game sound effects ****/
/*********************************************************************/

void AdjustEngineSound(ShipData *playerShip, TrackCamera *camera)
{
	int16_t vol;
	int16_t vol2;
	int16_t pitch;
	int16_t noteChange;
	int16_t breakPoint;
	return;

	/******************************************/
	/****  				ENGINE 2					****/
	/******************************************/

	vol = (ENG_AIR_INTAKE_VOL * playerShip->speed) >> 15;
	if(vol > ENG_AIR_INTAKE_VOL) vol = ENG_AIR_INTAKE_VOL;

	/*** 			Pitch Increment 0x1000 				***/
	/***	Pitch was started at 42 + 4 octaves    ***/

	pitch = playerShip->speed >> 2;

	SetPitch(ENG_AIR_INTAKE, 0x0800 + pitch);
	SetVolume(ENG_AIR_INTAKE, vol, vol);

	vol = 375 + ((ENG_THRUST_VOL * playerShip->thrust_mag) / playerShip->max_thrust);

	pitch = (0x7ff * playerShip->thrust_mag) / playerShip->max_thrust;
	pitch += playerShip->speed >> 5;

	/*** 			Pitch Increment 2048 				***/
	/***	Pitch was started at 55 + 2 octaves    ***/

	SetPitch(ENG_THRUST, 0x1000 + pitch);
	SetVolume(ENG_THRUST, vol, vol);

	vol = 750 + ((REMOTE_ENG_VOL * playerShip->thrust_mag) / playerShip->max_thrust);

	pitch = (0x3ff * playerShip->thrust_mag) / playerShip->max_thrust;
	pitch += playerShip->speed >> 5;

	/*** 			Pitch Increment 2048 				***/
	/***	Pitch was started at 55 + 2 octaves    ***/

	SetPitch(REMOTE_ENG, 0x0800 + pitch);
	SetVolume(REMOTE_ENG, vol, vol);

	/******************************************/
	/****  	  TURBULANCE & AIR BRAKES		****/
	/******************************************/

	/******* Needs re-balancing ********/

	vol = playerShip->speed >> 1 ;
	/*
	s_attr[TURBULENCE].volume.left = (vol>>2) + ((vol * playerShip->r_brake_rot)>>9);
	s_attr[TURBULENCE].volume.right = (vol>>2) + ((vol * playerShip->l_brake_rot)>>9);
	SpuSetVoiceAttr(&(s_attr[TURBULENCE]));
	*/
	SetVolume(TURBULENCE, (vol>>2) + ((vol * playerShip->r_brake_rot)>>9), (vol>>2) + ((vol * playerShip->l_brake_rot)>>9));

}

int32_t InitCD(CdlLOC* loc, int32_t trackNo)
{
	uint8_t	param;
	int32_t		i;

	ntoc = CdGetToc(loc);
	return(ntoc);
}

int32_t InitCrowdSound(Object** prm, int16_t prmCount, Object **lostad, int32_t stadCount)
{
	int32_t 			i, j;
	Object      *obj;

	return 0;
	for( i = 0; i < prmCount; i++)
	{
		obj = prm[ i ];
		while ( obj )
		{
			if(!strncmp(obj->name, "lost", 4))
			{
				lostad[stadCount] = obj;
				stadCount++;

				//				printf("Stadium = %s \n", &(obj->name) );
			}

			obj = obj->next;
		}

	}
	return(stadCount);
}



void AdjustCrowdSound(Object **lostad, int32_t stadCount)
{
	int16_t 		channel;

	int32_t i;
	VECTOR	distance;
	int16_t 	vol;
	int32_t 		mag, mag2;
	int32_t		chosen[2];
	int32_t 		smallestMag[2];
	VECTOR	pos;

	return;
	if (error == NO_SOUND)
		return;

	channel =-1;

	smallestMag[0] = 10000;
	smallestMag[1] = 10000;
	chosen[0] = -1;
	chosen[1] = -1;

	for(i = 0; i < stadCount; i++)
	{
		distance.vx = (lostad[i]->skeleton->relative.t[0] + cameraPtr->camPos->relative.t[0])>>3;
		distance.vy = (lostad[i]->skeleton->relative.t[1] + cameraPtr->camPos->relative.t[1])>>3;
		distance.vz = (lostad[i]->skeleton->relative.t[2] + cameraPtr->camPos->relative.t[2])>>3;

		mag = GetMagnitude(distance);

		if(mag < smallestMag[0])
		{
			if(chosen[0] != -1)
			{
				smallestMag[1] = smallestMag[0];
				chosen[1] = chosen[0];
			}
			smallestMag[0] = mag;
			chosen[0] = i;
		}
		else if(mag < smallestMag[1])
		{
			smallestMag[1] = mag;
			chosen[1] = i;
		}
	}


	if(smallestMag[0] < 8192)
	{
		pos.vx = lostad[chosen[0]]->skeleton->relative.t[0];
		pos.vy = lostad[chosen[0]]->skeleton->relative.t[1];
		pos.vz = lostad[chosen[0]]->skeleton->relative.t[2];

		//AdjustDirectionalNote(CROWD, 0, -1, pos );
	}
	else
	{
		channel = WhatChannel(CROWD);

		if (IsPlaying(channel))
		{
			KillNote(CROWD);
		}
	}

	if(smallestMag[1] < 8192)
	{
		pos.vx = lostad[chosen[1]]->skeleton->relative.t[0];
		pos.vy = lostad[chosen[1]]->skeleton->relative.t[1];
		pos.vz = lostad[chosen[1]]->skeleton->relative.t[2];

		//AdjustDirectionalNote(CROWD2, 0, -1, pos );

	}
	else
	{
		channel = WhatChannel(CROWD2);

		if (IsPlaying(channel))
		{
			KillNote(CROWD2);
		}
	}

#ifdef CROWD_SAMPLE_ON

	channel = WhatChannel(CROWD);

	if (IsPlaying(channel))
	{
		SetVolume(CROWD,
	}
	if (channel !=-1)
	{
		if (error!=NO_SOUND)
		{
			if((SampTable[CROWD].Playing !=0) && (SampTable[CROWD].Loop !=0))
			{
				SfxEffectPitch(SampTable[Channel2Sample[channel]].PlayID,ConvPitch(chanTable[channel].s_attr.pitch));
				SfxEffectVol(SampTable[Channel2Sample[channel]].PlayID,ConvVol(chanTable[channel].s_attr.volume.left,chanTable[channel].s_attr.volume.right));
				SfxEffectPan(SampTable[Channel2Sample[channel]].PlayID,ConvPan(chanTable[channel].s_attr.volume.left,chanTable[channel].s_attr.volume.right));
			}
		}
	}
#endif
}
