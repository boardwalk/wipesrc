/*********************************************************
*
* File: Sfx.h
*
* Header file to sfx.c
*
**********************************************************
*
*********************************************************/

#ifndef _SFX_H_INCLUDED_
#define _SFX_H_INCLUDED_

/*
 * Card Types
 */
#define NO_CARD						 106
#define SOUNDBLASTER				 1
#define ULTRASOUND					 2
#define AUTO_DETECT				   9

/*
 * Mix Types
 */
#define MONO 		1
#define STEREO 	2
#define PHASED	3
#define QSOUND 	4
#define DOLBY		5

/*
 * Sample Rates
 */
#define SR_11K	11025
#define SR_22K	22050

/*
 * Pitch defines
 */
#define OCTAVE_ABOVE				512
#define	NORMAL_PITCH				256
#define	OCTAVE_BELOW				128

/*
 * Flags
 */
#define ALL_OFF				0
#define LOOP_ON	 			1
#define SURROUND_ON		2
#define PROLOGIC_ON		4

/*
 * Stereo reverse defines
 */
//#define NORMAL			0
//#define REVERSE			1

/* 
 * Reverb Effects Types
 */
#define ECHO				0
#define DELAY_1			1
#define DELAY_2			2
#define REVERB      3

/*
 * Effect Flags
 */
#define SET_FLAG		1
#define RESET_FLAG  0

/*
 * Sound Driver features
 */
#define SFX_STEREO	0x00000001
#define SFX_PHASED  0x00000002
#define SFX_QSOUND  0x00000004
#define SFX_DOLBY		0x00000008
#define SFX_16BIT	  0x00000010
#define SFX_22KHZ		0x00000020
#define SFX_REVERB	0x00000040
#define SFX_VOLUME	0x00000080
#define SFX_CDVOL		0x00000100
#define SFX_CHAT		0x00000200
#define SFX_CALLBACK 0x00000400


/*
 * Function prototypes 
 */
uint16 SfxInit(uint16 wCard,uint16 wMixType,uint16 wSampRate, void *hwnd);

uint16 SfxInitReverb(uint16 wReverbType,uint16 wDelay);
void SfxReverbOn(void);
void SfxReverbOff(void);
void SfxSetReverbDepth(uint8 bDepth);
void SfxUnInitReverb(void);

/* Chat stuff not working yet! */
uint16 SfxInitChat(void (*CallBack)(uint8 *pbBufPtr,uint32 dwCode));
void SfxChatOn(void);
void SfxChatOff(void);
void SfxChatPlay(uint8 *pbMsgPtr,uint32 dwMsgSize);
void SfxUnInitChat(void);

void SfxSetStereoReverse(uint8 bFlag);
uint16 SfxSetMaxEffects(uint16 wNum);

int16 SfxLoadEffect(char *pcFilename); 
uint16 SfxUnLoadEffect(int16 wSampleId);
uint16 SfxGetFxBank(char *filename);
int16 SfxOpenFxBank(int16 *pwFirstId);
void SfxCloseFxBank(int16 wBank);


void SfxSetVolume(uint8 bVolume);
void SfxSetCdVolume(uint8 bVolume);
uint32 SfxGetFeatures(void);
uint16 SfxGetData(int16 wPlayId,uint8 *pbVolume,int8 *pbPan, 
									uint8 *pbLoop, uint16 *pwPitch,uint8 *pbPerCent);
void SfxSetCallBack(void (*CallBack)(uint16 wPlayId));

void SfxPlay(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags);
int16 SfxPlayId(int16 wSampleId,uint8 bVolume,uint8 bLRPan,uint8 bFBPan,
						 uint16 wPitch,uint8 bFlags);
int16 SfxChainId(int16 wPrevId,int16 wSampleId,uint8 bVolume,
								 uint8 bLRPan,uint8 bFBPan,uint16 wPitch,uint8 bFlags);

void SfxStop(int16 wPlayId);
void SfxEffectVol(int16 wPlayId,uint8 bVolume);
void SfxEffectPitch(int16 wPlayId,uint16 wPitch);
void SfxEffectPan(int16 wPlayId,uint16 bPan);
void SfxEffectFBPan(int16 wPlayId,uint16 bFBPan);
void SfxEffectFlags(int16 wPlayId,uint16 wFlag,uint8 bState);

void SfxOn(void);
void SfxOff(void);
void SfxPause(uint8 bFlag);										 
void SfxContinue(void);
void SfxDoSfx(void);
void SfxUnInit(uint8 bFlag);

uint16 SfxSndTrkInit(uint16 wBuffers,uint16 wSampleRate);
void SfxSndTrkUnInit(void);
void SfxSndTrkSetCallBack(void (*CallBack)(void));
void SfxSndTrkBuffer(uint8 *pbBufPtr,uint32 dwBufLen);
uint16 SfxSndTrkNumFull(void);
void SfxSndTrkStart(void);
void SfxSndTrkStop(void);


/*
 * Sound effect code assertion
 */
#ifdef SFXDEBUG
	
	#ifndef _SFXASSERT_
	#define _SFXASSERT_
		
		void _SfxAssert(char *,uint32,char *);	/* prototype */
	
		#define SFXASSERT(w,s) 		\
			if((w))	 								\
						;									\
			else										\
				_SfxAssert(__FILE__,__LINE__,s)


	#endif	/* _SFXASSERT_ */
#else
	#ifndef SFXASSERT

		#define SFXASSERT(w,s) 	//

	#endif	/* SFXASSERT */

#endif	/* DEBUG */


#endif /* _SFX_H_INCLUDED_ */
