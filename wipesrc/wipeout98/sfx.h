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
uint16_t SfxInit(uint16_t wCard,uint16_t wMixType,uint16_t wSampRate);

uint16_t SfxInitReverb(uint16_t wReverbType,uint16_t wDelay);
void SfxReverbOn(void);
void SfxReverbOff(void);
void SfxSetReverbDepth(uint8_t bDepth);
void SfxUnInitReverb(void);

/* Chat stuff not working yet! */
uint16_t SfxInitChat(void (*CallBack)(uint8_t *pbBufPtr,uint32_t dwCode));
void SfxChatOn(void);
void SfxChatOff(void);
void SfxChatPlay(uint8_t *pbMsgPtr,uint32_t dwMsgSize);
void SfxUnInitChat(void);

void SfxSetStereoReverse(uint8_t bFlag);
uint16_t SfxSetMaxEffects(uint16_t wNum);

int16_t SfxLoadEffect(const char *pcFilename);
uint16_t SfxUnLoadEffect(int16_t wSampleId);
uint16_t SfxGetFxBank(const char *filename);
int16_t SfxOpenFxBank(int16_t *pwFirstId);
void SfxCloseFxBank(int16_t wBank);


void SfxSetVolume(uint8_t bVolume);
void SfxSetCdVolume(uint8_t bVolume);
uint32_t SfxGetFeatures(void);
uint16_t SfxGetData(int16_t wPlayId,uint8_t *pbVolume,int8_t *pbPan,
									uint8_t *pbLoop, uint16_t *pwPitch,uint8_t *pbPerCent);
void SfxSetCallBack(void (*CallBack)(uint16_t wPlayId));

void SfxPlay(int16_t wSampleId,uint8_t bVolume,uint8_t bLRPan,uint8_t bFBPan,
						 uint16_t wPitch,uint8_t bFlags);
int16_t SfxPlayID(int16_t wSampleId,uint8_t bVolume,uint8_t bLRPan,uint8_t bFBPan,
						 uint16_t wPitch,uint8_t bFlags);
int16_t SfxChainId(int16_t wPrevId,int16_t wSampleId,uint8_t bVolume,
								 uint8_t bLRPan,uint8_t bFBPan,uint16_t wPitch,uint8_t bFlags);

void SfxStop(int16_t wPlayId);
void SfxEffectVol(int16_t wPlayId,uint8_t bVolume);
void SfxEffectPitch(int16_t wPlayId,uint16_t wPitch);
void SfxEffectPan(int16_t wPlayId,uint16_t bPan);
void SfxEffectFBPan(int16_t wPlayId,uint16_t bFBPan);
void SfxEffectFlags(int16_t wPlayId,uint16_t wFlag,uint8_t bState);

void SfxOn(void);
void SfxOff(void);
void SfxPause(uint8_t bFlag);
void SfxContinue(void);
void SfxDoSfx(void);
void SfxUnInit(uint8_t bFlag);

uint16_t SfxSndTrkInit(uint16_t wBuffers,uint16_t wSampleRate);
void SfxSndTrkUnInit(void);
void SfxSndTrkSetCallBack(void (*CallBack)(void));
void SfxSndTrkBuffer(uint8_t *pbBufPtr,uint32_t dwBufLen);
uint16_t SfxSndTrkNumFull(void);
void SfxSndTrkStart(void);
void SfxSndTrkStop(void);


/*
 * Sound effect code assertion
 */
#ifdef SFXDEBUG

	#ifndef _SFXASSERT_
	#define _SFXASSERT_

		void _SfxAssert(char *,uint32_t,char *);	/* prototype */

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
