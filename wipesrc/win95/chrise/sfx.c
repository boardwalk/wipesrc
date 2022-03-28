/********************************************************
*
* File Sfx.c
*
* Non sound card specific PC sound effects routines.
*
********************************************************
*															 
* $Log:   P:/ridger/ridger/SFX.C_v  $
 * 
 *    Rev 1.0   13 Jul 1995 14:39:40   DaveS
 * Put Back after we stopped being naughty children
* 
*    Rev 1.9   08 Feb 1995 14:02:02   AndrewC
* Added: 
* Stereo panning.
* Full SoundBlaster support.
* 
* 
*    Rev 1.8   09 Jan 1995 15:45:02   KimW
* No change.
* 
*    Rev 1.7   22 Dec 1994 10:15:48   NeilC
* dont know
* 
*    Rev 1.6   15 Dec 1994 16:42:12   KimW
* uncommented important line!
* 
*    Rev 1.5   18 Nov 1994 15:16:38   AndrewC
* Changed $Log$ so it works.
* 
* 
*
********************************************************/

//
//	Added 11025kHz option
//
									 

/*
 * Include files
 */
#include "c:\watcom\h\stdio.h"
#include "c:\watcom\h\stdlib.h"
#include "c:\watcom\h\string.h"
#include <dos.h>
#include <conio.h>
#include <math.h>

#include "utypes.h"
#include "sbsfx.h"
#include "sfx.h"			
#include "sfxmix.h"
#include "sfxdma.h"
#include "sfxrevrb.h"

/*
 * Typedefs and structures
 */															  

typedef struct{
	char pcMagic[6];			/* SNDFX */
	uint16 wFlags;			 	/* Compression flags */
  uint32 dwSrate;				/* Sample Rate */
  uint16 wChannels;			/* Number of channels, 1 or 2 */
  uint32 dwDataLen1;		/* Length of audio data in each section */
  uint32 dwDataLen2;		/*  */
  uint32 dwDataLen3;		/*  */
  uint32 dwDataLen4;		/*  */
	uint32 dwImgPos;			/* Start of Qsound image data */
	uint32 dwImgLen1;			/* Length of Image data in each section */
	uint32 dwImgLen2;			/*  */
	uint32 dwImgLen3;			/*  */
	uint32 dwImgLen4;			/*  */
  uint32 dwRes1;				/* Unused */
  uint32 dwRes2;				/* Unused for future expansion ??? */
  uint32 dwRes3;				/* Unused */
}CMP_HEADER;

static struct effectItem{
	uint16 wSrate;
	uint16 wFlags;
	uint32 dwSmpLen;
	uint8 *pbSmpPtr;
	uint8 *pbImgPtr;
};

static struct playListStruct{
	uint16 wPlayId;
	uint16 wFlags;
	uint8  bVolume;
	uint8  bPan;
	uint32 dwSmpIndx;			
	uint32 dwSmpIndx2;
	uint32 dwSmpLen;
	uint8 *pbSmpPtr;
	uint16 wSrate;
	uint16 wIncrement;
	uint16 wIncIndx;
	uint16 wIncIndx2;
 	struct playListStruct *pNext;
 	struct playListStruct *pPrev;
 	struct playListStruct *pChain;
	uint8 *pbImgPtr;		/* QSound image file */
};

struct idMapStruct
{
 	struct playListStruct *pItem;
 	uint16 wFlags;
};

/* 
 * #defines and macros
 */

/** programmable interrupt ports **/
#define PIC_COMMAND		0x20
#define PIC_MASK			0x21

#define MIX_BUFFER_SIZE 0x8000	/* default size of Mix Buffer in bytes */
#define RADIO_BUFFER_SIZE 800
#define RADIO_BLOCK_SIZE  (RADIO_BUFFER_SIZE/2)
#define RADIO_MESSAGE_SIZE 0x10000	/* 64k receive buffer */

#define TOTAL_EFFECTS	 	64		//ajy (What a silly number)
#define ACTIVE_EFFECTS 		8
#define VIRTUAL_EFFECTS		16

#define VOLUME_LEVELS 32		/* change in sfxmix.c also */
 																				 
/* effect flags */
#define ACTIVE 			0x01
#define LOOP				0x02
#define CONTROL			0x04
#define CHAINED			0x08
#define SURROUND		0x10
#define QSND_IMAGE	0x20
#define CALLBACK		0x80		/* set by mixing routine */

/* IdMap flags */
#define NEW					0x01
#define MOVE				0x02
#define DELETE			0x04
#define REMOVE			0x08					  

/*
 * Macros
 */
#define ABS(x)	((x) > 0 ? (x) : -(x))		
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define SIGN(x)  ((x) >= 0 ? (1) : (-1))

/*
 * Time bar macros
 * #undef TIME_BAR to remove
 */
//#define TIME_BAR
#ifdef TIME_BAR
	#define WHITE outp(0x3c8,0);outp(0x3c9,40);outp(0x3c9,40);outp(0x3c9,40); 
	#define RED   outp(0x3c8,0);outp(0x3c9,40);outp(0x3c9,0);outp(0x3c9,0);   
	#define GREEN outp(0x3c8,0);outp(0x3c9,0);outp(0x3c9,40);outp(0x3c9,0);	  
	#define BLUE  outp(0x3c8,0);outp(0x3c9,0);outp(0x3c9,0);outp(0x3c9,40);   
	#define PINK 	outp(0x3c8,0);outp(0x3c9,40);outp(0x3c9,25);outp(0x3c9,25); 
	#define BLACK outp(0x3c8,0);outp(0x3c9,0);outp(0x3c9,0);outp(0x3c9,0);    
#else
	#define WHITE 	/**/
	#define RED   	/**/
	#define GREEN 	/**/
	#define BLUE  	/**/
	#define PINK		/**/
	#define BLACK 	/**/
#endif

/*
 *  inline assembler functions to switch interrupts 
 */
#pragma aux InterruptsOff = \
 						"cli";
 
#pragma aux InterruptsOn = \
						"sti";

/*
 * Function prototypes 
 */

/** pointer to old interrupt routine **/
static void interrupt (far *OldInterruptRoutine)(void);

/** pointers to specific sound card functions **/
static void (*DoSfxStart)(uint16 wSampleRate,uint16 wChannels,
													uint32 dwStartAddr,uint16 wBlockSize);
static void (*DoSfxRecStart)(uint16 wSampleRate,uint32 dwStartAddr,
													uint16 wBlockSize);
static void (*DoSfxAcknowledgeInt)(void);
static void (*DoSfxAcknowledgePlayInt)(void);
static void (*DoSfxAcknowledgeRcrdInt)(void);
static void (*DoSfxSetVolume)(uint8 bVolume);
static void (*DoSfxUnInit)(void);

/** pointer to mix routine **/
static uint16 (*DoMix)(struct playListStruct **ppItem);
static void (*DoMixCopy)(void);

/** pointer to callback function **/
void (*DoCallBack)(uint16 wPlayId);

/***** Non exported function prototypes *****/
void InterruptsOff(void);
void InterruptsOn(void);
static void AddItem(struct playListStruct *pItem);
static void RemoveItem(struct playListStruct *pItem);
static void SetSfxInt(void);
static void interrupt ServSfxInt(void);
static void ClrSfxInt(void);

/*
 * Exported globals (to sfxmix.c) 
 */
void *ppvBufferG[2];				/* array of two pointers to lo/hi DMA buffer */
uint16 wMaxActiveG;
uint16 wMaxBitsG;
int32 *pdwMixBufG;
uint32 dwMixBufSizeG;
uint32 dwMixBufSamplesG;
uint32 dwMixBufFramesG;
uint32 dwDmaBufSizeG;
uint32 dwDmaBlockSizeG;
uint32 dwDmaBlockSamplesG;
volatile uint16 wBufferFlagG;

uint16 wSampleRateG;
uint32 dwDebug = 0;

/*
 * Chat related stuff 
 */
void (*DoChatCallBack)(uint8 *pbBufPtr,uint32 dwCode);
void *ppvChatBufG[2];				/* chat DMA buffer pointers */
uint8 *pbChatMsgPtrG;				/* pointer to message buffer */
uint32 dwChatMsgIndxG;				/* index to message buffer */
uint32 dwChatMsgSizeG;				/* length of data in message buffer */
uint8 bChatReadyG;

/*
 *  Non Exported Global Variables
 */
static uint8 bOldMaskG;
static uint32 dwDosMemSelectorG;
static struct effectItem effectTableG[TOTAL_EFFECTS];
static struct playListStruct *pHeadG;
static struct playListStruct *pTailG;
static struct playListStruct *pItemG;

static struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];

static uint8 pbBufClearG[2];

static volatile uint16 wBufferSwitchG;

static uint8 bChannelsG;
static uint8 bInitOkG = 0;
static uint8 bSfxOnG = 0;
static uint8 bCallBackOnG = 0;
static uint8 bReverbSetG = 0;
static uint8 bReverbOnG = 0;

static uint16 wIrqG;
static uint16 wListUpdateG = 0;

static uint8 bMixTypeG = MONO;

static uint8 bStereoLock;
static uint8 bStereoReverse;

static uint8 bChatInitG;
static uint8 bChatOnG;
static uint32 dwChatSelectorG;

#define DEBUG_ON 0
#if DEBUG_ON == 1
static volatile uint32 dwSfxDoSfxCalledG = 0;
#endif


/********************************************
*
* uint16 SfxInit(uint16 wCard)
* 
* uint16 wCard : Sound card type installed
*							 : use #defines in sfx.h
*
*	uint16 wMixType : type of mix requested
*									: use #defines in sfx.h
*
* uint16 wSampRate : sample rate effects mixed at
*									: either 11025kHz or 22050kHz
*
* returns uint16 : error flag 0=init OK, 1=fail
*
**************************************
*
* Initialises the sound effects routines and
* sound card. 
*
**************************************
* 
* Must be called before any other sound effects routine
*
*************************************/
uint16 SfxInit(uint16 wCard,uint16 wMixType,uint16 wSRate)
{		
	extern uint8 bInitOkG;		   
	extern uint8 bSfxOnG;
	extern uint16 wIrqG;
	extern struct playListStruct *pHeadG;
	extern struct playListStruct *pTailG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint8 pbBufClearG[2];
	extern uint16 wListUpdateG;
	extern volatile uint16 wBufferSwitchG;
	extern uint32 dwDosMemSelectorG;
	extern uint16 wMaxBitsG;
	extern uint16 wMaxActiveG;
	extern uint32 dwMixBufSizeG;
	extern uint32 dwMixBufSamplesG;
	extern uint32 dwMixBufFramesG;
	extern uint32 dwDmaBufSizeG;
	extern uint32 dwDmaBlockSizeG;
	extern uint32 dwDmaBlockSamplesG;
	extern void *ppvBufferG[2];	
	extern int32 *pdwNixBufG;
	extern volatile uint16 wBufferFlagG;
	extern uint16 wSampleRateG;

	uint8 bError;
	uint16 wIndx;
	uint16 wChannels;
	uint16 wMaxChannels;
	uint32 dwZero;

	bInitOkG = 0;
	bStereoLock = NORMAL;
	bStereoReverse = 0;		/* normal stereo */
	wSampleRateG = wSRate;

	
	/*																  
	 * Check valid mix type given 
	 */
	if((wMixType < MONO) || (wMixType > QSOUND))
	{
		return 1;		/* invalid mixtype requested */
	}
	
 	/*
	 * Initialise card 
	 */
 	switch(wCard)
 	{
 		case SOUNDBLASTER:
 			bError = SBlasterSfxInit(&wIrqG,&wMaxChannels,&wMaxBitsG);
			DoSfxStart = SBlasterSfxStart;
			DoSfxRecStart = SBlasterRecStart;
			DoSfxAcknowledgePlayInt = SBlasterAckInt;
			DoSfxAcknowledgeRcrdInt = SBlasterAckRecInt;
			DoSfxSetVolume = SBlasterSfxSetVolume;
			DoSfxUnInit = SBlasterSfxUnInit;

			if(wMaxBitsG == 8 && wMaxChannels == 2)
			{
				/*
				 * SB-Pro card installed
				 * Fix stereo reverse by toggling reverse flag
				 * this is a bit of a quick fix, but it works!
				 */
				bStereoReverse = 1;
			}
 			break;
		case ULTRASOUND:
			bError = 1;
 		default:
			bError = 1;
 			break;
 	}
					
 	if(bError == 1)
 	{
 		return 2;
 	}

	/*
	 * Compare requirements against card capabilities
	 */
	wChannels = 1;
	if(wMixType >= STEREO)
		wChannels = 2;

	if(wChannels > wMaxChannels)
		wChannels = wMaxChannels;

	/*
	 * Calculate Mix buffer sizes 
	 * (dependent on sample rate)
	 */
	switch(wSampleRateG)
	{
		case 22050:
			//printf("22050Hz ");
			dwMixBufSizeG = MIX_BUFFER_SIZE;
			break;
		case 11025:
			//printf("11025Hz ");
			dwMixBufSizeG = MIX_BUFFER_SIZE/2;
			break;
		default:
			return 3;		/* invalid sample rate */
	}
	dwMixBufSamplesG = dwMixBufSizeG/4;
	dwMixBufFramesG = dwMixBufSamplesG/2;

	/*
	 * Calculate Dma buffer sizes 
	 * (directly related to card capabilities and requirements)
	 */
	if(wChannels == 2)
		dwDmaBufSizeG = dwMixBufSizeG;
	else
		dwDmaBufSizeG = dwMixBufSizeG/2;

	if(wMaxBitsG == 8)
	{
		dwDmaBufSizeG = dwDmaBufSizeG/2;			/* 1 byte per sample */
		dwDmaBlockSizeG = dwDmaBufSizeG/2;
		dwDmaBlockSamplesG = dwDmaBlockSizeG;
	}
	else
	{
		dwDmaBlockSizeG = dwDmaBufSizeG/2;
		dwDmaBlockSamplesG = dwDmaBlockSizeG/2;
	}

  /*
	 * allocate DMA buffer 
	 */
	ppvBufferG[0] = (void *)RecursiveAlloc(dwDmaBufSizeG,&dwDosMemSelectorG);
 	if(ppvBufferG[0] == 0)
 	{
 		return 4;		/* failed to allocate memory */
 	}

	/*
	 * Clear buffer 
 	 */
	if(wMaxBitsG == 8)
		dwZero = 0x80808080;
	else
		dwZero = 0;

	for(wIndx=0;wIndx<dwDmaBufSizeG/4;wIndx++)
	{
		*((uint32 *)ppvBufferG[0]+wIndx) = 0;
	}
	
	/** split buffer into two **/
	if(wMaxBitsG == 16)
	{
		ppvBufferG[1] = (int16 *)ppvBufferG[0] + dwDmaBlockSamplesG;
  }
	else
	{
		ppvBufferG[1] = (uint8 *)ppvBufferG[0] + dwDmaBlockSamplesG;
	}

 	/*
	 * allocate mix buffer
	 */
	pdwMixBufG = (int32 *)malloc(dwMixBufSizeG);
    																												 
	/*
	 * Initialise sample data structures
	 */
	pHeadG = NULL;
	pTailG = NULL;
	for(wIndx=0;wIndx<TOTAL_EFFECTS;wIndx++)
  {
	 	effectTableG[wIndx].dwSmpLen = 0;
		effectTableG[wIndx].pbSmpPtr = NULL;
	}
	for(wIndx=0;wIndx<VIRTUAL_EFFECTS;wIndx++)
	{
		pIdMapG[wIndx].pItem = NULL;
		pIdMapG[wIndx].wFlags = 0;
	}

	wMaxActiveG = ACTIVE_EFFECTS;
 	pbBufClearG[0] = 0;
 	pbBufClearG[1] = 0;

	/*
	 * Select Mixing routine 
	 */
	if(wChannels == 1)
	{
		//printf("Mono Mixing\n");
		DoMix = MonoMix;
		bMixTypeG = MONO;
	}
	else
	{
		switch(wMixType)
		{
			case STEREO:
				//printf("Stereo Mixing\n");
				DoMix = QStereoMix;
				bMixTypeG = STEREO;
				break;
			case PHASED:
				//printf("Phased Stereo Mixing\n");
				DoMix = PhaseMix;
				bMixTypeG = PHASED;
				break;
			case QSOUND:
				//printf("QSound Mixing\n");
				DoMix = QStereoMix;
				bMixTypeG = QSOUND;
				break;
		}
	}

	if(MixSetup(bMixTypeG))
	{
		return 5;		/* Failed */
	}

	/*
	 * Select Mix Copy routine
	 */
	if(wMaxBitsG == 16)
	{
		if(wChannels == 2)
		{	
			DoMixCopy = Mix2Dma16Stereo;
		}
		else 
		{
			DoMixCopy = Mix2Dma16Mono;
		}
	}
	else
	{
		if(wChannels == 2)
		{
			DoMixCopy = Mix2Dma8Stereo;
		}
		else
		{
			DoMixCopy = Mix2Dma8Mono;
		}
	}

	/*
	 * initialise global variables 
	 */
	wBufferFlagG = 1;
	wBufferSwitchG = 0;
														   
	bInitOkG = 1;		/* Sfx Initialised OK */
	bSfxOnG = 0;		/* Sfx NOT turned on */
	bCallBackOnG = 0;

	bChannelsG = wChannels;

	wListUpdateG = 0;

	/*
	 * Setup interrupt routine
	 */
	DoSfxAcknowledgeInt = DoSfxAcknowledgePlayInt;
	SetSfxInt();

	/*
	 * Start DMA transfer cycle
	 */
	DoSfxStart(wSampleRateG,wChannels,(uint32)ppvBufferG[0],dwDmaBlockSamplesG);
	
	return 0;
}														 

/*******************************************************
*
* uint16 SfxInitReverb(uint16 wReverbType,uint16 wDelay)
*
* uint16 wReverbType: Use #defines in Sfx.h
* uint16 wDelay: Delay time in milli seconds (Echo and Delay effects only)
*							 : Max 2000 (2 seconds)
*
* returns: uint16 : error flag 1=OK, 0 = fail
*
********************************************************
*
* Calls InitReverb to initialise reverb buffers with 
* reverb type. 
*
*******************************************************/
uint16 SfxInitReverb(uint16 wReverbType,uint16 wDelay)
{
	uint16 wError;
		
	/* impose limit on delay time */
	if(wDelay>2000)
		wDelay = 2000;

	wError = InitReverb(wReverbType,bChannelsG,wDelay);
	if(wError)
		return 1;

	bReverbSetG = 1;
	bReverbOnG = 0;

	return 0;
}

/*****************************************************
*
* void SfxReverbOn(void)
*
******************************************************
*
* If initialised this will enable the reverb processing
*
*****************************************************/
void SfxReverbOn(void)
{
	if(bReverbSetG)
		bReverbOnG = 1;
}

/******************************************************
* 
* void SfxReverbOff(void)
*
*******************************************************
*
* Turns off reverb processing and clears used buffers
*
******************************************************/
void SfxReverbOff(void)
{
	if(bReverbOnG)
	{
		bReverbOnG = 0;
		ClearReverbBuffers();
	}
}

/************************************
*
* void SfxSetReverbDepth(uint8 bDepth)
*
* uint8 bDepth: Depth of reverb effect on mix
*							: range 0 to 127
*
*************************************
*
* Sets the depth of reverb on the mix
*
************************************/
void SfxSetReverbDepth(uint8 bDepth)
{
	if(bReverbSetG)
		SetReverbDepth(bDepth);
}

/*******************************************************
*
* void SfxUnInitReverb(void)
*
********************************************************
*
* Deallocates reverb buffers 
*
*******************************************************/
void SfxUnInitReverb(void)
{
	if(bReverbSetG)
	{
		bReverbOnG = 0;
		bReverbSetG = 0;
		UnInitReverb();
	}
}

#if 0

/*
 * This is multiplayer network chat stuff which
 * is not quite working yet (at all infact)...
 */

uint16 SfxInitChat(void (*CallBack)(uint8 *pbBufPtr,uint32 dwCode))
{
	if(!bInitOkG)
		return 1;

	/*
	 * Allocate DMA record buffer
	 */
	ppvChatBufG[0] = RecursiveAlloc(RADIO_BUFFER_SIZE,
																 	&dwChatSelectorG);
	ppvChatBufG[1] = (uint8*)ppvChatBufG[0] + RADIO_BLOCK_SIZE;

	/*
	 * Setup the callback to the network stuff
	 */
	DoChatCallBack = CallBack;

	/* 
	 * Init some variables 
	 */
	dwChatMsgIndxG = 0;
	dwChatMsgSizeG = 0;
	bChatInitG 		= 1;
	bChatOnG 			= 0;
	bChatReadyG 		= 0;

	return 0;
}

void SfxChatOn(void)
{
	if(!bChatInitG || bChatOnG)
		return;

	/*
	 * Switch from playback to record mode 
	 */

	/* change interrupt acknowledgement function */
	DoSfxAcknowledgeInt = DoSfxAcknowledgeRcrdInt;
	DoSfxRecStart(5512,(uint32)ppvChatBufG[0],RADIO_BLOCK_SIZE);
	bChatOnG = 1;
}

void SfxChatOff(void)
{
	if(!bChatInitG || !bChatOnG)
		return;

	/*
	 * Switch from record to playback mode 
	 */

	/* change interrupt acknowledgement function */
	ClrSfxInt();
	DoSfxAcknowledgeInt = DoSfxAcknowledgePlayInt;
	SetSfxInt();
	SBlasterSfxUnInit();
	DoSfxStart(wSampleRateG,bChannelsG,(uint32)ppvBufferG[0],dwDmaBlockSamplesG);
	bChatOnG = 0;
}

void SfxChatPlay(uint8 *pbMsgPtr,uint32 dwMsgSize)
{
	if(!bChatInitG)
		return;

	bChatReadyG = 1;
	pbChatMsgPtrG = pbMsgPtr;
	dwChatMsgSizeG = dwMsgSize;
	dwChatMsgIndxG = 0;
}

void SfxUnInitChat(void)
{
	if(!bChatInitG)
		return;

	if(bChatOnG)
	{
		SfxChatOff();
		bChatOnG = 0;
	}

	DosMemFree(dwChatSelectorG);
	free(pbChatMsgPtrG);
	bChatInitG = 0;
}	

#endif

/**************************************
*
* void SfxSetStereoReverse(uint8 bFlag)
*
* uint8 bFlag : Stereo reverse on/off
*
***************************************
*
* Sets stereo reverse flag so that the
* left and right channels are swapped.
*
* Note this doesn't just set the flag but
* checks a lock, since a fix for the SB-PRO
* card needs the reverse flag set for normal
* stereo.
*
***************************************
*
* Called by game code
*
**************************************/
void SfxSetStereoReverse(uint8 bFlag)
{
	extern uint8 bStereoLock;
	extern uint8 bStereoReverse;

	if(bFlag > 1)
		return;

	if(bFlag != bStereoLock)
	{
		/* Toggle reverse flag */
		bStereoReverse = !bStereoReverse;

		/* set lock */
		bStereoLock = bFlag;
	}
}
	

/****************************************
*
* uint16 SfxSetMaxEffects(uint16 wNum)
*
* uint16 wNum : maximum number of effects 
*							: that may be played back at 
*							: any one time.	range, 1-8
*
* returns: maximum effects to mix
*
*****************************************
*
* Sets the maximum number of effects that 
*	may be mixed, upto a maximum of 8
*
*****************************************
*
* Called by game routines
*
****************************************/
uint16 SfxSetMaxEffects(uint16 wNum)
{
	extern uint16 wMaxActiveG;

 	if(wNum <= ACTIVE_EFFECTS)
	{
		wMaxActiveG = wNum;
	}

 	return wMaxActiveG;
}


/****************************************
*
*	int16 SfxLoadEffect(char *pcFilename)
*
* char *pcFilename : pointer to filename
*
* returns : sample ID / error code
*					: natural numbers: sample Id
*					: negative numbers: error code:
*														: -1: not Init Ok
*														: -2: couldn't open file
*														: -3: incorrect file type
*														:	-4: incorrect sample spec
*														: -5: all sample IDs allocated
*														: -6: couldn't allocate memory
*
****************************************** 				
*
* This function loads a compressed sample into
* memory, returning an ID which may subsequently be 
* used to trigger that sample.
*
******************************************
*
* Called by: Game routine
*
* Depends on: SfxInit() must be successfully called previously
*
******************************************/
int16 SfxLoadEffect(char *pcFilename)
{
	extern uint8 bInitOkG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern uint16 wSampleRateG;

	uint32 dwSampSize;
	uint16 wSRate;
	uint16 wSampleId;
	uint16 wSample;
	CMP_HEADER header;
	FILE *pf;

 	if(!bInitOkG)
	{	
		return -1;
	}

	/*
	 * open compressed sound file
	 */
	pf = fopen(pcFilename,"rb");
  if(pf==NULL)
	{
		return -2;
 	}

 	/*
	 * read header information
	 */
	fread(&header,sizeof(CMP_HEADER),1,pf);

	/*
	 * Check wave type
	 */
	if(strncmp(header.pcMagic,"SNDFX",5))
	{
		return -3;
	}
	if(header.wChannels != 1)
	{
		return -4;
	}

	/*
	 * Find free slot
	 */
	wSampleId=0;
	while(effectTableG[wSampleId].pbSmpPtr != NULL)
	{
		wSampleId++;

		if(wSampleId > TOTAL_EFFECTS)
		{
			/*
			 * All slots are used
			 */
			return -5;	/* error */
		}
	}

	/*
	 * Determine which sample needs to be loaded
	 */
	wSample = 0;
	if(wMaxBitsG != 16)
	{
		wSample++;
	}
	if(header.dwSrate == 22050)
	{
		if(wSampleRateG == 11025)
		{
			wSample += 2;
		}
	}

	/*
	 * Prepare to load sample
	 */
	switch(wSample)
	{
		case 0:
			dwSampSize = header.dwDataLen1;
			wSRate = header.dwSrate;
			break;
		case 1:
			dwSampSize = header.dwDataLen2;
			fseek(pf,header.dwDataLen1,SEEK_CUR);
			wSRate = header.dwSrate;
			break;
		case 2:
			dwSampSize = header.dwDataLen3;
			fseek(pf,(header.dwDataLen1+header.dwDataLen2),SEEK_CUR);
			wSRate = header.dwSrate/2;
			break;
		case 3:
			dwSampSize = header.dwDataLen4;
			fseek(pf,(header.dwDataLen1+
							  header.dwDataLen2+
							  header.dwDataLen3),SEEK_CUR);
			wSRate = header.dwSrate/2;
			break;
	}

	/*
	 * Allocate memory for the sample 
	 */
	effectTableG[wSampleId].pbSmpPtr = (uint8 *)malloc(dwSampSize);
	if(effectTableG[wSampleId].pbSmpPtr == NULL)
	{
		return -6;	/* Couldn't allocate memory */
	}
	
	/*
	 * Fill in effect table
	 */
	effectTableG[wSampleId].dwSmpLen = dwSampSize;
	effectTableG[wSampleId].wSrate = wSRate;
	effectTableG[wSampleId].wFlags = 0;

	/*
	 * Load sample
	 */
	fread(effectTableG[wSampleId].pbSmpPtr,1,dwSampSize,pf);

	/*
	 * QSound?
	 */
	if((bMixTypeG == QSOUND) && (header.dwImgPos != 0))
	{
		/*
		 * Prepare to load sample
		 */
		fseek(pf,(sizeof(CMP_HEADER)+header.dwImgPos),SEEK_SET);
		switch(wSample)
		{
			case 0:
				dwSampSize = header.dwImgLen1;
				wSRate = header.dwSrate;
				break;
			case 1:
				dwSampSize = header.dwImgLen2;
				fseek(pf,header.dwImgLen1,SEEK_CUR);
				wSRate = header.dwSrate;
				break;
			case 2:
				dwSampSize = header.dwImgLen3;
				fseek(pf,(header.dwImgLen1+header.dwImgLen2),SEEK_CUR);
				wSRate = header.dwSrate/2;
				break;
			case 3:
				dwSampSize = header.dwImgLen4;
				fseek(pf,(header.dwImgLen1+
								  header.dwImgLen2+
								  header.dwImgLen3),SEEK_CUR);
				wSRate = header.dwSrate/2;
				break;
		}

		/*
		 * Allocate memory for the sample 
		 */
		effectTableG[wSampleId].pbImgPtr = (uint8 *)malloc(dwSampSize);
		if(effectTableG[wSampleId].pbImgPtr == NULL)
		{
			return -6;	/* Couldn't allocate memory */
		}

		/*
		 * Load sample
		 */
		fread(effectTableG[wSampleId].pbImgPtr,1,dwSampSize,pf);
		effectTableG[wSampleId].wFlags |= QSND_IMAGE;
	}
	
	fclose(pf);

 	return wSampleId;	
}


/*********************************************
*
* uint16 SfxUnLoadEffect(int16 wSampleId)
*
* int16 wSampleId: ID of sample to unload
*
* returns: error flag: 1 = error, 0 = OK
*
******************************************
*
* Removes a loaded effect from memory, freeing
* the sample ID which may later be used for another effect
*
******************************************
*
* Called by: 	game routine
*
* Depends on: SfxInit() must have been successfully called
*
*****************************************/
uint16 SfxUnLoadEffect(int16 wSampleId)
{
	extern uint8 bInitOkG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	struct playListStruct *pItem;
	uint16 wPlayId;

	if(!bInitOkG)
	{
		return 0;
	}

	/*
	 * Deactivate sample if playing
	 */
	for(wPlayId=0;wPlayId<VIRTUAL_EFFECTS;wPlayId++)
	{
		if(pIdMapG[wPlayId].pItem)
		{
			pItem = pIdMapG[wPlayId].pItem;

			if(pItem->pbSmpPtr == effectTableG[wSampleId].pbSmpPtr)
			{
				pIdMapG[wPlayId].wFlags = DELETE;
				wListUpdateG++;
			}
		}
	}
 
	/*
	 * free memory and reset data
	 */
	if(effectTableG[wSampleId].pbSmpPtr != NULL)
	{
		free(effectTableG[wSampleId].pbSmpPtr);
	}
	effectTableG[wSampleId].dwSmpLen = 0;
	effectTableG[wSampleId].pbSmpPtr = NULL;

	if(effectTableG[wSampleId].wFlags & QSND_IMAGE)
	{
		if(effectTableG[wSampleId].pbImgPtr != NULL)
		{
			free(effectTableG[wSampleId].pbImgPtr);
		}
		effectTableG[wSampleId].pbImgPtr = NULL;
	}

	effectTableG[wSampleId].wFlags = 0;

	return 0;
}

/*********************************************
*
* void SfxSetVolume(uint8 bVolume)
*
* uint8 bVolume: effects volume level, range 0-255
*
**********************************************
*
* Sets the overall sound effects volume level.
* range 0-255, 255 = max volume.
*
**********************************************
*
* Called by: game routines
*
* Depends on:  SfxInit() must have been successfully called
*
*********************************************/
void SfxSetVolume(uint8 bVolume)
{	
	extern uint8 bInitOkG;

	if(bInitOkG)
	{
		DoSfxSetVolume(bVolume);
	}
}

/**********************************************
* 
* uint16 SfxGetData(int16 wPlayId,uint8 *pbVolume, int8 *pbPan,
*										uint8 *pbLoop, uint16 *pwPitch, uint8 *pbPerCent)
*
* int16 wPlayId: 	 	Effect Identifier.
*	uint8 *pbVolume: 	current volume level
* uint8 *pbPan:    	current pan position
* uint8 *pbLoop:	 	loop flag 
* uint16 *pwPitch: 	current pitch
* uint8 *pbPerCent:	current position in sample length eg. 90% complete
*
**********************************************
*
* Returns data about an effect currently playing.
* The per cent complete data is an approximate value.
*
* returns: error code: 0 data valid, 1 error, effect doesn't exist
*
**********************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called
*
*********************************************/
uint16 SfxGetData(int16 wPlayId,uint8 *pbVolume,int8 *pbPan, 
									uint8 *pbLoop, uint16 *pwPitch,uint8 *pbPerCent)
{
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	struct playListStruct *pItem;

	pItem = pIdMapG[wPlayId].pItem;
	if(pItem == NULL)
	{
		return 1;		/* effect doesn't exist */
	}

	/*
	 * Get Data
	 */
	*pbVolume = pItem->bVolume << 4;		/* scale back to 255 range */
	*pbPan 		= pItem->bPan;
	*pwPitch  = pItem->wIncrement;
	if(pItem->wFlags & LOOP)
	{
		*pbLoop = 1;
	}
	else
	{
		*pbLoop = 0;
	}

	/* Calculate % complete */
	*pbPerCent = (uint8)( (pItem->dwSmpIndx*100) / pItem->dwSmpLen);

	return 0;	/* no error */
}

/*******************************************************
*
* void SfxSetCallBack(void (*CallBack)(uint16 wPlayId));
* 
*
* void (*CallBack)(uint16 wPlayId)) : pointer to callback
*					function in calling application.
*
********************************************************
*
* Installs a callback function in the calling application
* which will be called on completion of a sound effect triggered
* by either SfxPlayId() or SfxChainId, ie those effects for
* which an ID was returned. The Id is passed as a parameter
* to the callback function.
*
********************************************************
*
* Called by: SfxSetCallBack : game code.
* 				 : DoCallBack	: called from SfxDoSfx()
*
* Depends On: SfxInit() must be successfully called first
*
*******************************************************/
void SfxSetCallBack(void (*CallBack)(uint16 wPlayId))
{
	extern uint8 bInitOkG;
	extern uint8 bCallBackOnG;

	if(bInitOkG)
	{
		/*
		 * Point to external callback function
		 */
		DoCallBack = CallBack;
		bCallBackOnG = 1;
	}
}


/********************************************************
*
* void SfxPlay(int16 wSampleId,uint8 bVolume,int8 bPan,uint16 wPitch,uint8 bFlags)
*
* int16 wSampleId: Id of sample to play
* uint8 bVolume: 	 playback volume level, 0-255, 255=max vol
* uint8  bPan:		 stereo pan position, 0 to 15, 0= full left, 15= full right
* uint16 wPitch:	 playback pitch. 256 = normal pitch, 
*																	 512 = octave higher
*																	 128 = octave below
* uint8 bFlags		 modifier flags, see sfx.h
*									 LOOP_ON flag has no effect here
*
**********************************************************
*
* Triggers an effect for playback.
* The effect cannot subsequently be controlled in any way.
*
**********************************************************
*
* Called by: game routine.
*
* Depends on: SfxInit() must have been called and sound effects
* turned on with SfxOn()
*
*********************************************************/
void SfxPlay(int16 wSampleId,uint8 bVolume,uint8 bPan,uint16 wPitch,uint8 bFlags)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;
	struct playListStruct *pNew;
	int16 wPlayId;

	if(!bInitOkG || !bSfxOnG || wSampleId < 0 || wSampleId > TOTAL_EFFECTS)
	{
		return;
	}

	/*
	 * Check selected sample Id exists
	 */
	if(effectTableG[wSampleId].pbSmpPtr == NULL)
	{
		return;		/* effect doesn't exist ! */
	}

	/*
	 * Look for a free play ID
	 */
	wPlayId = 0;
	while(pIdMapG[wPlayId].pItem != NULL)
	{
		wPlayId++;
		if(wPlayId == VIRTUAL_EFFECTS)
		{	
			return;
		}
	}

	/*
	 * Limit pan range
	 */
	bPan = MIN(bPan,30);

	/*
	 * Fix pan for stereo reverse
	 */
	if(bStereoReverse)
	{
		bPan = 30-bPan;
	}

	/*
	 * Setup new effect element
	 */
	pNew = (struct playListStruct *)malloc(sizeof(struct playListStruct));
	pNew->wFlags = ACTIVE;
	if(bFlags & SURROUND_ON)
	{
		pNew->wFlags |= SURROUND;
	}
	pNew->bVolume = bVolume >> 3;		/* scale to usable range */
	pNew->bPan = bPan;
	pNew->wIncrement = wPitch;
	pNew->wIncIndx = 0;
	pNew->wIncIndx2 = 0;
	pNew->pbSmpPtr = effectTableG[wSampleId].pbSmpPtr;
	pNew->dwSmpLen = effectTableG[wSampleId].dwSmpLen;
	pNew->dwSmpIndx = 0;
	pNew->dwSmpIndx2 = 0;
	pNew->wSrate = effectTableG[wSampleId].wSrate;
	pNew->wPlayId = wPlayId;
	pNew->pNext = NULL;
	pNew->pPrev = NULL;
	pNew->pChain = NULL;
	pNew->pbImgPtr = NULL;

	/*
	 * Setup QSound additions
	 */
	if(effectTableG[wSampleId].wFlags & QSND_IMAGE)
	{
		pNew->pbImgPtr = effectTableG[wSampleId].pbImgPtr;
		pNew->wFlags |= QSND_IMAGE;
	}

	/* Linked List needs updating */
	wListUpdateG++;

	/* point to new element */
	pIdMapG[wPlayId].pItem = pNew;
	pIdMapG[wPlayId].wFlags = NEW;

}

/********************************************************
*
* int16 SfxPlayId(int16 wSampleId,uint8 bVolume,int8 bPan,uint16 wPitch,uint8 bFlags)
*
* int16 wSampleId: Id of sample to play
* uint8 bVolume: 	 playback volume level, 0-255, 255=max vol
* uint8 bPan:			 stereo pan position, 0 to 15, 0= full left, 15= full right
* uint16 wPitch:	 playback pitch. 256 = normal pitch, 
*																	 512 = octave higher
*																	 128 = octave below
* uint8 bFlags:		 LOOP_ON | SURROUND_ON
*
* returns: a playback effect identifier. 
*
**********************************************************
*
* Triggers an effect for playback.
* The effect can subsequently be controlled using the returned playback Id.
*
* Mixing of effects is far more efficient when played at normal pitch (256)
* 
* Effects triggered by this function must be killed later by SfxStop()
* to free the effect ID for reuse.
*
**********************************************************
*
* Called by: game routine.
*
* Depends on: SfxInit() must have been called and sound effects
* turned on with SfxOn()
*
*********************************************************/
int16 SfxPlayId(int16 wSampleId,uint8 bVolume,uint8 bPan,uint16 wPitch,uint8 bFlags)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;
	struct playListStruct *pNew;
	int16 wPlayId;

	if(!bInitOkG || !bSfxOnG || wSampleId < 0 || wSampleId > TOTAL_EFFECTS)
	{
		return -1;
	}

	/*
	 * Check selected sample Id exists
	 */
	if(effectTableG[wSampleId].pbSmpPtr == NULL)
	{
		return -3;		/* effect doesn't exist ! */
	}


	/*
	 * Look for a free play ID 
	 */
	wPlayId = 0;
	while(pIdMapG[wPlayId].pItem != NULL)
	{
		wPlayId++;

		if(wPlayId == VIRTUAL_EFFECTS)
		{
			return -2;	/* all virtual effects allocated */
		}
	}

	/*
	 * Limit pan range
	 */
	bPan = MIN(bPan,30);

	/*
	 * Fix pan for stereo reverse
	 */
	if(bStereoReverse)
	{
		bPan = 30-bPan;
	}

	/*
	 * SetUp new effect element
	 */
	pNew = (struct playListStruct *)malloc(sizeof(struct playListStruct));
	pNew->wFlags = ACTIVE | CONTROL;
	if(bFlags & LOOP_ON)
	{
		pNew->wFlags |= LOOP;
	}
	if(bFlags & SURROUND_ON)
	{
		pNew->wFlags |= SURROUND;
	}

	pNew->bVolume = bVolume >> 3;
	pNew->bPan = bPan;
	pNew->wIncrement = wPitch;
	pNew->wIncIndx = 0;
	pNew->wIncIndx2 = 0;
	pNew->pbSmpPtr = effectTableG[wSampleId].pbSmpPtr;
	pNew->dwSmpLen = effectTableG[wSampleId].dwSmpLen;
	pNew->wSrate = effectTableG[wSampleId].wSrate;
	pNew->dwSmpIndx = 0;
	pNew->dwSmpIndx2 = 0;
	pNew->wPlayId = wPlayId;
	pNew->pNext = NULL;
	pNew->pPrev = NULL;
	pNew->pChain = NULL;
	pNew->pbImgPtr = NULL;
	
	/*
	 * Setup QSound additions
	 */
	if(effectTableG[wSampleId].wFlags & QSND_IMAGE)
	{
		pNew->pbImgPtr = effectTableG[wSampleId].pbImgPtr;
		pNew->wFlags |= QSND_IMAGE;
	}

	/* Linked List needs updating */
	wListUpdateG++;

	/* point to element */
	pIdMapG[wPlayId].pItem = pNew;
	pIdMapG[wPlayId].wFlags = NEW;

	return wPlayId;
}


/********************************************************
*
* int16 SfxChainId(int16 wPrevId,int16 wSampleId,uint8 bVolume,
									 uint8 bPan,uint16 wPitch,uint8 bFlags)
*
* int16 wPrevId: 	 Id of effect this is chained to.
* int16 wSampleId: Id of sample to play
* uint8 bVolume: 	 playback volume level, 0-255, 255=max vol
* uint8 bPan:			 stereo pan position, 0 to 15, 0=full left, 15=full right
* uint16 wPitch:	 playback pitch. 256 = normal pitch, 
*																	 512 = octave higher
*																	 128 = octave below
* uint8 bFlags:		 LOOP_ON | SURROUND_ON
*
* returns: a playback effect identifier. 
*
**********************************************************
*
* Allows an effect to be triggered by the completion of a previous effect.
* A play list of effects may be built up, so that as soon as one effect is
* complete, another is triggered.
* The effect can subsequently be controlled using the returned playback Id.
*
* Mixing of effects is far more efficient when played at normal pitch (256)
* 
* Effects triggered by this function must be killed later by SfxStop()
* to free the effect ID for reuse.
*
**********************************************************
*
* Called by: game routine.
*
* Depends on: SfxInit() must have been called and sound effects
* turned on with SfxOn()
*
*********************************************************/
int16 SfxChainId(int16 wPrevId,int16 wSampleId,uint8 bVolume,
								 uint8 bPan,uint16 wPitch,uint8 bFlags)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;
	struct playListStruct *pNew,*pPrev;
	int16 wPlayId;

	if(!bInitOkG || !bSfxOnG || wSampleId < 0 || wSampleId > TOTAL_EFFECTS)
	{
		return -1;
	}

	/*
	 * Check for valid wPrevId
	 */
	if(wPrevId < 0 || wPrevId > VIRTUAL_EFFECTS)
	{
		return -3;
	}
	if(pIdMapG[wPrevId].pItem == NULL)
	{
		return -3;	/* previous effect does not exist */
	}

	/*
	 * Check selected sample Id exists
	 */
	if(effectTableG[wSampleId].pbSmpPtr == NULL)
	{
		return -3;		/* effect doesn't exist ! */
	}


	/*
	 * Look for a free play ID 
	 */
	wPlayId = 0;
	while(pIdMapG[wPlayId].pItem != NULL)
	{
		wPlayId++;

		if(wPlayId == VIRTUAL_EFFECTS)
		{
			return -2;	/* all virtual effects allocated */
		}
	}											 

	/*
	 * Limit pan range
	 */
	bPan = MIN(bPan,30);

	/*
	 * Fix pan for stereo reverse
	 */
	if(bStereoReverse)
	{
		bPan = 30-bPan;
	}

	/*
	 * SetUp new effect element
	 */
	pNew = (struct playListStruct *)malloc(sizeof(struct playListStruct));
	pNew->wFlags = ACTIVE | CONTROL;
	if(bFlags & LOOP_ON)
	{
		pNew->wFlags |= LOOP;
	}
	if(bFlags & SURROUND_ON)
	{
		pNew->wFlags |= SURROUND;
	}
	pNew->bVolume = bVolume >> 3;
	pNew->bPan = bPan;
	pNew->wIncrement = wPitch;
	pNew->wIncIndx = 0;
	pNew->wIncIndx2 = 0;
	pNew->pbSmpPtr = effectTableG[wSampleId].pbSmpPtr;
	pNew->dwSmpLen = effectTableG[wSampleId].dwSmpLen;
	pNew->wSrate = effectTableG[wSampleId].wSrate;
	pNew->dwSmpIndx = 0;
	pNew->dwSmpIndx2 = 0;
	pNew->wPlayId = wPlayId;
	pNew->pNext = NULL;
	pNew->pPrev = NULL;
	pNew->pChain = NULL;
	pNew->pbImgPtr = NULL;

	/*
	 * Setup QSound additions
	 */
	if(effectTableG[wSampleId].wFlags & QSND_IMAGE)
	{
		pNew->pbImgPtr = effectTableG[wSampleId].pbImgPtr;
		pNew->wFlags |= QSND_IMAGE;
	}

	/*
	 * Chain from previous effect
	 */
	pPrev = pIdMapG[wPrevId].pItem;
	pPrev->pChain = pNew;
	pPrev->wFlags |= CHAINED;

	/* Linked List needs updating */
	wListUpdateG++;

	/* point to element */
	pIdMapG[wPlayId].pItem = pNew;
	pIdMapG[wPlayId].wFlags = 0;

	return wPlayId;
}	

/*************************************************************
* 
* void SfxStop(int16 wPlayId)
*
* int16 wPlayId: ID of an effect previously triggered
*
**************************************************************
*
* Function to stop and free the ID of an effect triggered 
* using either SfxPlayId() or SfxChainId()
*
* This must be called after an ID'd effect is playing
* to avoid filling all of the available ID's 
*
**************************************************************
* 
* Called by: 	game routine
*
* Depends on: SfxInit() must have been successfully called
*
*************************************************************/
void SfxStop(int16 wPlayId)
{
	extern uint8 bInitOkG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;

	if(!bInitOkG || pIdMapG[wPlayId].pItem == NULL)
	{
		return;
	}

	pIdMapG[wPlayId].wFlags = DELETE;
	wListUpdateG++;

}

/****************************************************
*
* void SfxEffectVol(int16 wPlayId,uint8 bVolume)
*
* int16 wPlayId: ID of effect to control
*	uint8 bVolume: new volume level
*
****************************************************
*
* Sets a new volume level for a currently playing
* (or chained) effect.
*
****************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called
* SfxPlayId() or SfxChainId() must have been called, and
* its return parameter used as the play ID for this function.
*
*****************************************************/
void SfxEffectVol(int16 wPlayId,uint8 bVolume)
{
	extern uint8 bInitOkG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;
	struct playListStruct *pItem;

	if(!bInitOkG || pIdMapG[wPlayId].pItem == NULL)
	{
		return;
	}

 	pItem = pIdMapG[wPlayId].pItem;

	if(!(pItem->wFlags & CONTROL) || !(pItem->wFlags & ACTIVE))
	{
		return;		/* not a controllable effect, or effect finished ! */
	}

	/*
	 * update volume
	 */
	pItem->bVolume = bVolume >> 3;

	/* indicate that item may need to move */
	pIdMapG[wPlayId].wFlags = MOVE;
	wListUpdateG++;

}

/*******************************************************
*
* void SfxEffectPitch(int16 wPlayId,uint16 wPitch)
*
* int16 wPlayId: ID of effect to controll
*	uint16 wPitch: new volume level
*
****************************************************
*
* Sets a new pitch for a currently playing
* (or chained) effect.
*
* Note: Effect mixing is most efficient at normal pitch (256)
*
****************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called
* SfxPlayId() or SfxChainId() must have been called, and
* its return parameter used as the play ID for this function.
*
*****************************************************/
void SfxEffectPitch(int16 wPlayId,uint16 wPitch)
{
	extern uint8 bInitOkG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	struct playListStruct *pItem;

	if(!bInitOkG || pIdMapG[wPlayId].pItem == NULL)
	{
		return;
	}

	pItem = pIdMapG[wPlayId].pItem;

	if(!(pItem->wFlags & CONTROL) || !(pItem->wFlags & ACTIVE))
	{
		return;
	}

	/*
	 * Update Pitch 
	 */
	pItem->wIncrement = wPitch;

}

/*******************************************************
*
* void SfxEffectPan(int16 wPlayId,uint8 bPan)
*
* int16 wPlayId: ID of effect to controll
*	uint8 bPan: new pan position
*
****************************************************
*
* Sets a new pan for a currently playing
* (or chained) effect.
*
****************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called
* SfxPlayId() or SfxChainId() must have been called, and
* its return parameter used as the play ID for this function.
*
*****************************************************/
void SfxEffectPan(int16 wPlayId,uint8 bPan)
{
	extern uint8 bInitOkG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	struct playListStruct *pItem;

	if(!bInitOkG || pIdMapG[wPlayId].pItem == NULL)
	{
		return;
	}

	pItem = pIdMapG[wPlayId].pItem;

	if(!(pItem->wFlags & CONTROL) || !(pItem->wFlags & ACTIVE))
	{
		return;
	}

	/*
	 * Limit pan range
	 */
	bPan = MIN(bPan,30);

	/*
	 * Fix pan for stereo reverse
	 */
	if(bStereoReverse)
	{
		bPan = 30-bPan;
	}

	/*
	 * Update Pan
	 */
	pItem->bPan = bPan;

}

/*******************************************************
*
* void SfxEffectFlags(int16 wPlayId,uint8 bFlag,uint8 bState)
*
* int16 wPlayId: ID of effect to controll
*	uint8 bFlag: Flag to set
* uint8 bState: new state : SET_FLAG or RESET_FLAG
*
****************************************************
*
* Sets an effect flag
*
****************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called
* SfxPlayId() or SfxChainId() must have been called, and
* its return parameter used as the play ID for this function.
*
*****************************************************/
void SfxEffectFlags(int16 wPlayId,uint16 wFlag,uint8 bState)
{
	extern uint8 bInitOkG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	struct playListStruct *pItem;
	uint16 wNewFlag;

	if(!bInitOkG || pIdMapG[wPlayId].pItem == NULL)
	{
		return;
	}

	pItem = pIdMapG[wPlayId].pItem;

	if(!(pItem->wFlags & CONTROL) || !(pItem->wFlags & ACTIVE))
	{
		return;
	}

	/*
	 * Convert flags from user selectable to internal
	 */
	switch(wFlag)
	{
		case SURROUND_ON:
			wNewFlag = SURROUND;
			break;
		case LOOP_ON:
			wNewFlag = LOOP;
			break;
		default:
			return;			/* flag not recognised */
	}

	/*
	 * Update flags
	 */
	if(bState == RESET_FLAG)
	{
		pItem->wFlags &= ~wNewFlag;		/* reset */
	}
	else
	{
		pItem->wFlags |= wNewFlag;			/* set */
	}
}		

/******************************************************
*
* void SfxOn(void)
*
*******************************************************
*
* Turns sound effects on. This function must be called
* for sound effects to be mixed together and played back.
*
*******************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called.
*
******************************************************/ 
void SfxOn(void)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	if(bInitOkG)
	{
		bSfxOnG = 1;
	}
}

/******************************************************
*
* void SfxOff(void)
*
*******************************************************
*
* Turns off sound effects. Triggered sound effects will
* no longer be mixed and played back.
*
*******************************************************
*
* Called by: game routine
*
* Depends on: SfxInit() must have been successfully called.
*
******************************************************/
void SfxOff(void)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern struct playListStruct *pHeadG;
	extern struct playListStruct *pTailG;
	extern uint16 wMaxBitsG;
	extern uint32 dwDmaBufSizeG;
	extern void *ppvBufferG[2];

	struct playListStruct *pItem;
	int16 wPlayId;
	uint16 wIndx;
	uint32 dwZero;

	if(bInitOkG)
	{
		bSfxOnG = 0;

		/*
		 * Deallocate all effects
		 */
 		for(wPlayId=0;wPlayId<VIRTUAL_EFFECTS;wPlayId++)
		{
			if(pIdMapG[wPlayId].pItem != NULL)
			{
				pItem = pIdMapG[wPlayId].pItem;
				pIdMapG[wPlayId].pItem = NULL;
				pIdMapG[wPlayId].wFlags = 0;

				RemoveItem(pItem);
				free(pItem);
			}
		}
		
		pHeadG = NULL;
		pTailG = NULL;

		/*
	 	 * Clear buffer 
	 	 */
		if(wMaxBitsG == 8)
			dwZero = 0x80808080;
		else
			dwZero = 0;
		for(wIndx=0;wIndx<dwDmaBufSizeG/4;wIndx++)
		{	
			*((uint32 *)ppvBufferG[0]+wIndx) = dwZero; 
		}
	}
}

/********************************************************
* 
* void SfxDoSfx(void)
*
*********************************************************
*
* This function performs the mixing and playback of 
* multiple sound effects. It must be called repeatedly
* from an timer interrupt or similar running at about 80Hz or faster.
*
* Execution time is dependent on the number of effects to 
* be mixed, the pitch of those effects, and the length
* of buffer used. The #define MIX_BUF_SIZE may need to be
* adjusted to configure this function to its calling frequency
* If called at high frequencies the buffer size should be
* reduced for faster, more frequent execution.
*
*********************************************************
*
* Called by: game code, regularly in timer interrupt (?)
*
* Depends On: SFX must be initialised with SfxInit() and
* turned on with SfxOn().
*
*******************************************************/		  
void SfxDoSfx(void)
{
	extern uint8 bInitOkG;
	extern uint8 bSfxOnG;
	extern uint8 bCallBackOnG;
	extern volatile uint16 wBufferSwitchG;
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint16 wListUpdateG;
	extern volatile uint16 wBufferSwitchG;
	extern struct playListStruct *pHeadG;
	extern struct playListStruct *pItemG;
	extern uint8 pbBufClearG[2];
	extern volatile uint16 wBufferFlagG;
	extern uint16 wMaxActiveG;
	extern uint32 dwMixBufFramesG;
	extern int32 *pdwMixBufG;

	struct playListStruct *pNext,*pChain;
	uint16 wIndx;
	int16 wPlayId;

  if(!wBufferSwitchG || !bInitOkG || !bSfxOnG)
	{
		return;
  }

	WHITE

	wBufferSwitchG = 0;  

	wIndx = 0;
	while(wListUpdateG>0)
	{
		/*
		 * search IdMap for items needing change 			  
		 */
		if(pIdMapG[wIndx].pItem && pIdMapG[wIndx].wFlags)
		{	
			switch(pIdMapG[wIndx].wFlags)
			{
				case NEW:
 			 	  /*
			 	 	 * Effect is added to the linked list
			 	 	 */
			 		AddItem(pIdMapG[wIndx].pItem);
			 		pIdMapG[wIndx].wFlags &= ~NEW;
					break;
				case MOVE:
					/*
				 	 * Effect volume changed, so must be moved to new position
				 	 */
					RemoveItem(pIdMapG[wIndx].pItem);																	 
					AddItem(pIdMapG[wIndx].pItem);
					pIdMapG[wIndx].wFlags &= ~MOVE;
					break;
				case REMOVE:
			  	/* 
				   * Effect complete, but not killed. 
				 	 * It can be removed from the list, but its ID not freed
				 	 */
					RemoveItem(pIdMapG[wIndx].pItem);
					pIdMapG[wIndx].wFlags &= ~REMOVE;
					break;
				case DELETE:
			  	/*
			 	 	 * Effect complete and released
			 	 	 * It may be removed from the list 
			 	   * and its ID freed
			 	 	 */
					if(pIdMapG[wIndx].pItem->wFlags & ACTIVE)
					{
						RemoveItem(pIdMapG[wIndx].pItem);
					}
			 		free(pIdMapG[wIndx].pItem);
			 		pIdMapG[wIndx].pItem = NULL;
			 		pIdMapG[wIndx].wFlags &= ~DELETE;
					break;
			}

			wListUpdateG--;			 
		}
		wIndx++;
	}

	/*
	 * Record (radio) or playback (SFX)
	 */
	if(bChatOnG)
	{
		/*
		 * Record mode
		 * Call network stuff with pointer to DMA buffer
		 */
		DoChatCallBack(ppvChatBufG[wBufferFlagG],0);
			
		pItemG = pHeadG;			/* allows virtual mixing to continue */
	}
	else
	{
		/*
		 * Playback mode
		 */
		if(pHeadG == NULL)						 
		{
			if((pbBufClearG[wBufferFlagG] == 0) && (bReverbOnG == 0))
			{
				ClearDmaBuf();
				pbBufClearG[wBufferFlagG] = 1;
			}

			if(bReverbOnG)
			{	

				RED		 
				ClearMixBuf();
				GREEN
				AddReverb();
				BLUE
				DoMixCopy();
	  		pbBufClearG[wBufferFlagG] = 0;
			}

			BLACK

			return; /* no effects to mix */
		}							   

		/*												  
		 * Mix effects
		 * call assembler mixing routine
		 */

		/*
		 * Note: pItem has been made global, rather than local to avoid some weird
		 * bug that caused a crash when SfxDoSfx() was called by an interrupt routine.
		 * When evaluated in MonoMix() &pItem didn't point to pHead as intended, 
		 * but to some other address. The code worked fine when called from a while loop
		 */
		pItemG = pHeadG;

		wListUpdateG = 0;

		RED

		if(wMaxActiveG>0)
		{
	 		wListUpdateG = DoMix(&pItemG);
		}

		GREEN

		/*
		 * Perform reverberation/delay effects etc on mix buffer
		 */
		if(bReverbOnG)
		{
			AddReverb();
		}
			
		BLUE

		if(bChatReadyG)
		{
			MixChat();
		}
										
		/* copy mix buffer to DMA buffer */
		DoMixCopy();

		BLACK
	}

	/*
	 * 'Virtual mix' remaining effects 
	 *
	 * ALL THIS NEEDS UPDATING FOR NEW MIXES.... SOON!!!!!
	 */
	while(pItemG)
	{
		/*
  	 * increment sample index on silent samples
	   */
		if(pItemG->wIncrement == 256)
		{
				
 			pItemG->dwSmpIndx += dwMixBufFramesG;
 			if(pItemG->dwSmpIndx > pItemG->dwSmpLen)
 			{
				pItemG->wFlags |= CALLBACK;

 				if(pItemG->wFlags & LOOP)
 				{
 					pItemG->dwSmpIndx -= pItemG->dwSmpLen;
 				}
 				else				  
 				{
 					pItemG->wFlags &= ~ACTIVE;
 					wListUpdateG++;
 				}
 			}
 		}
		else
		{
			pItemG->dwSmpIndx += (dwMixBufFramesG<<8)/pItemG->wIncrement;
			if(pItemG->dwSmpIndx > pItemG->dwSmpLen)
			{	
				pItemG->wFlags |= CALLBACK;
				
				if(pItemG->wFlags & LOOP)
				{
					pItemG->dwSmpIndx	= ((pItemG->dwSmpIndx-pItemG->dwSmpLen)
															 *pItemG->wIncrement) >> 8;
				}
				else
				{
					pItemG->wFlags &= ~ACTIVE;
					wListUpdateG++;
				}
			}
		}

 		pItemG = pItemG->pNext;
 	}

	/*
	 * Send callbacks for completed or looped effects
	 */
	if(bCallBackOnG)
	{
		pItemG = pHeadG;
		while(pItemG)
		{
			if(pItemG->wFlags & CALLBACK)
			{
				DoCallBack(pItemG->wPlayId);
	
				pItemG->wFlags &= ~CALLBACK;		/* reset flag */
			}
			pItemG = pItemG->pNext;
		}
	}

	/*
	 * Manage inactive effects in play list
	 * only if list not currently being updated
	 * by other functions.
	 */
	pItemG = pHeadG;
	while(wListUpdateG>0)
	{
		/*																													 
	 	 * Search for inactive items and delete
	   */
	 	pNext = pItemG->pNext;
		if(!(pItemG->wFlags & ACTIVE))
 		{
			if(pItemG->wFlags & CHAINED)
			{					 
				/* Add next effect in chain to linked list */
				pChain = pItemG->pChain;
				AddItem(pChain);
			}

 			if(!(pItemG->wFlags & CONTROL))
 			{
 				/* 
 		   	 * effect complete and not controlled so remove
 		     */
 				wPlayId = pItemG->wPlayId;
 				RemoveItem(pItemG);
 				free(pItemG);
 				pIdMapG[wPlayId].pItem = NULL;
				pIdMapG[wPlayId].wFlags = 0;
 			}
 			else
 			{
 				/*
 				 * controlled effect cannot be deleted from IdMap
 				 * it can be removed from list, but not freed
 				 */
 				RemoveItem(pItemG);
 			}
			wListUpdateG--;
 		}
	 	pItemG = pNext;
	}

	/* mark buffer as full */
  pbBufClearG[wBufferFlagG] = 0;
}

/**************************************************************
*
* void SfxUnInit(void)
*
***************************************************************
* 
* This function performs a graceful closedown of the sound
* effects routines and sound card.
*
* Any loaded effects are removed, and all sample and play Id's freed.
*
***************************************************************
*
* Called by: game code
*
* Depends on: SfxInit() must have been successfully called.
*
**************************************************************/
void SfxUnInit(void)
{
	extern uint8 bInitOkG;
	extern struct effectItem effectTableG[TOTAL_EFFECTS];
	extern struct idMapStruct pIdMapG[VIRTUAL_EFFECTS];
	extern uint32 dwDosMemSelectorG;
	struct playListStruct *pItem;
	uint16 wIndx;
	int16 wPlayId;

	if(!bInitOkG)
	{
		return;
	}

	/* set initialised flag to zero */
	bInitOkG = 0;

	/*
	 * deallocate sample memory
	 */
	for(wIndx=0;wIndx<TOTAL_EFFECTS;wIndx++)
	{
		if(effectTableG[wIndx].pbSmpPtr != NULL)
		{
			free(effectTableG[wIndx].pbSmpPtr);
		}
	}

	/*
	 * Deallocate linked list structure
	 */
	for(wPlayId=0;wPlayId<VIRTUAL_EFFECTS;wPlayId++)
	{
		if(pIdMapG[wPlayId].pItem != NULL)
		{
 			pItem = pIdMapG[wPlayId].pItem;
			pIdMapG[wPlayId].pItem = NULL;
			pIdMapG[wPlayId].wFlags = 0;

			RemoveItem(pItem);
			free(pItem);
		}
	}

	/* free mixing buffer */
	free(pdwMixBufG);

	/** free DOS memory **/
	DosMemFree(dwDosMemSelectorG);

	DoSfxUnInit();
	ClrSfxInt();
}

/***********************************************************/ 
/*************** Non exported SFX functions ****************/
/***********************************************************/

/**********************************************************
*
* void AddItem(struct playListStruct *pItem)
*
* struct playListStruct *pItem: pointer to element to add
*														    to list.
*
***********************************************************
*
* Adds an item to the linked list data structure.
*
***********************************************************
*
* Called by: SfxDoSfx() 
*
* Depends on: item must have been allocated and initialised by
* one of SfxPlay(), SfxPlayId() or SfxChainId()
*
**********************************************************/
void AddItem(struct playListStruct *pItem)
{
	extern struct playListStruct *pHeadG;
	extern struct playListStruct *pTailG;
	struct playListStruct *pCurr,*pOld;
	uint16 wPlaced;

	/*
	 * Link element in list
	 */
	wPlaced = 0;
	if(pHeadG == NULL)
	{
		/*
		 * No elements currently in list
		 */
		pItem->pNext = NULL;
		pItem->pPrev = NULL;
		pHeadG = pItem;
		pTailG = pItem;
		wPlaced = 1;
	}
	else
	{
		/*
		 * Search for correct place in list
		 */
		pCurr = pHeadG;
		pOld = NULL;
		while(pCurr && !wPlaced)
		{
			if(pItem->bVolume < pCurr->bVolume)
			{
				pOld = pCurr;
				pCurr = pCurr->pNext;
			}
			else
			{
				if(pCurr->pPrev)
				{
					/* 
					 * New middle element
					 */
					pCurr->pPrev->pNext = pItem;
					pItem->pNext = pCurr;
					pItem->pPrev = pCurr->pPrev;
					pCurr->pPrev = pItem;
					wPlaced = 1;
				}
				else
				{
					/*
					 * New first element
					 */
					pItem->pNext = pCurr;
					pItem->pPrev = NULL;
					pCurr->pPrev = pItem;
					pHeadG = pItem;
					wPlaced = 1;
				}
			}
		}
		if(!wPlaced)
		{
			/*
			 * New last element
			 */
			pOld->pNext = pItem;
			pItem->pNext = NULL;
			pItem->pPrev = pOld;
			pTailG = pItem;
		}
 	}
}

/*****************************************************
*
* void RemoveItem(struct playListStruct *pItem)
*
* struct playListStruct *pItem: pointer to item to remove
*
*****************************************************
*
* Removes an item from the linked list data structure.
*
*****************************************************
*
* Called by:	SfxDoSfx() and SfxUnInit()
*
* Depends on: Item must have been added to the list 
* by AddItem()
*
****************************************************/
void RemoveItem(struct playListStruct *pItem)
{
	extern struct playListStruct *pHeadG;
	extern struct playListStruct *pTailG;

	/*
	 * remove back link
	 */
	if(pItem->pPrev)
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	else
	{
		pHeadG = pItem->pNext;
		if(pHeadG)
		{
			pHeadG->pPrev = NULL;
		}
	}

	/*
	 * remove forward link
	 */
	if(pItem->pNext)
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}
	else
	{
		pTailG = pItem->pNext;
		if(pTailG)
		{
			pTailG->pNext = NULL;
		}
	}

	pItem->pPrev = NULL;
	pItem->pNext = NULL;
}

/*******************************************************
*
* void SetSfxInt(void)
*
********************************************************
*
* Sets up an interrupt called on completion of transfer of
* half a DMA buffer to the sound card.
*
********************************************************
*
* Called by: SfxInit()
*
* Depends on: nothing
*
*******************************************************/
void SetSfxInt(void)
{
	extern uint16 wIrqG;
	extern uint8 bOldMaskG;

	uint16 wMask;

	/** set new mask **/
	wMask = 1;					/* bit 0 */
	wMask <<= wIrqG;		/* shift to IRQ number */
	wMask = ~wMask;			/* invert */

	InterruptsOff();	/* turn off interrupts */

	/** store old interrupt **/
	OldInterruptRoutine = _dos_getvect(8+wIrqG);

	/** set interrupt **/
	_dos_setvect(8+wIrqG,ServSfxInt);
	
	/** get current interrupt mask **/
  bOldMaskG = inp(PIC_MASK);

	wMask &= bOldMaskG;	/* AND with current mask */
										/* makes sure interrupt is enabled */

	/** write new mask back **/
	outp(PIC_MASK,wMask);

 	InterruptsOn();		/* turn on interrupts */
}

/***************************************************
*
* void interrupt ServSfxInt(void)
*
****************************************************
*
* Called on completion of transfer of half the DMA
* buffer to the sound card.
* Sets a global flag to indicate data transfer has
* taken place.
*
***************************************************
*
* Called by: Sound Card via interrupt controller
*
* Depends on: Interrupt must be set up by SetSfxInt()
*
**************************************************/
void interrupt ServSfxInt(void)
{
	extern volatile uint16 wBufferSwitchG;
	extern volatile uint16 wBufferFlagG;

	/** set flag to indicate end of DSP block reached **/
	wBufferSwitchG = 1;
	wBufferFlagG = !wBufferFlagG;

  /** acknowledge interrupt with DSP **/
 	DoSfxAcknowledgeInt();

	/** output End Of Interrupt (EOI) command **/
  outp(PIC_COMMAND,0x20);
}

/**************************************************
*
* void ClrSfxInt(void)
*
***************************************************
*
* Restores the interrupt routine on the interrupt
*	used by the sound card.
*
***************************************************
*
* Called by: SfxUnInit()
*
* Depends on: SetSfxInt() must have been called to
* set the interrupt handler previously
*
**************************************************/
void ClrSfxInt(void)
{
	extern uint8 bOldMaskG;
	extern uint16 wIrqG;

  InterruptsOff();			/* turn off interrupts */
			 
	/** restore old interrupt **/
	_dos_setvect(8+wIrqG,OldInterruptRoutine);
														 
  /** restore previous mask **/
	outp(PIC_MASK,bOldMaskG);		

	InterruptsOn();			/* turn on interrupts */
}


			
/************************ Debug / Testing functions ***********************/

void ShowList(void)
{
	struct playListStruct *pItem;
	
	pItem = pHeadG;

	printf("wPlayId:  ");
	while(pItem)
	{
		printf("%6d  ",pItem->wPlayId);
		pItem = pItem->pNext;
	}

	/***
	pItem = pHead;
	printf("\nwVolume:  ");
	while(pItem)
	{
		printf("%6d  ",pItem->wVolume);
		pItem = pItem->pNext;
	}
								  
	pItem = pHead;
	printf("\ndwSmpLen: ");
	while(pItem)
	{
		printf("%6ld  ",pItem->dwSmpLen);
		pItem = pItem->pNext;
	}

	pItem = pHead;
	printf("\ndwSmpIndx:");
	while(pItem)
	{
		printf("%6ld  ",pItem->dwSmpIndx);
		pItem = pItem->pNext;
	}
	**/

	printf("\n");
	
}

	
void ShowIdMap(void)
{
	uint16 wIndx;

	for(wIndx=0;wIndx<VIRTUAL_EFFECTS;wIndx++)
	{
		if(pIdMapG[wIndx].pItem)
		{
			printf("%3d",wIndx);
		}
		else
			printf("  .");
	}
	printf("\n");
}



