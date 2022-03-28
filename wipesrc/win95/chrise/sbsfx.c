/***************************************************
*
*	File: sbsfx.c
*
* Contains all soundblaster specific sound effects setup
*
****************************************************
*
* $Log:   P:/ridger/ridger/sbsfx.c_v  $
 * 
 *    Rev 1.1   18 Jul 1995 15:39:16   DaveS
 * No changes (whoops)
 * 
 *    Rev 1.0   13 Jul 1995 14:39:36   DaveS
 * Put Back after we stopped being naughty children
 * 
 *    Rev 1.2   08 Feb 1995 14:10:36   AndrewC
 * Works for all SoundBlasters (I think).
 * SB-Pro stereo is left/right swapped though (!?)
 * 
 *    Rev 1.1   18 Nov 1994 15:08:54   AndrewC
 * Updated to drive more Soundblasters, works OK with SoundBlaster 2,16
 * and AWE32. Some code included for Pro card, but unlikely to work since
 * it is untested. Some bugs with cards below version 2 (clicks).
 * 
*
***************************************************/


/*
 * Include Files
 */

#include "c:\watcom\h\stdio.h"
#include "c:\watcom\h\stdlib.h"
#include "c:\watcom\h\string.h"
#include <dos.h>
#include <conio.h>

#include "utypes.h"
#include "sfxdma.h"


/*
 * #defines and macros.
 */
#define EIGHT_BIT_TEST 0

/** Macros **/
#define SBCPort(x)		((x)+wSbcBaseAddxG)
#define MPUPort(x)		((x)+wMpuBaseAddxG)
#define MK_20BIT(s,o) 	( ((uint32)(s) << 4) + (uint32)(o) )

/** SBC port offsets **/
#define RESET_PORT		6
#define READ_DATA			0xA
#define WRITE_DATA		0xC
#define WRITE_STATUS  0xC
#define READ_STATUS   0xE																 

#define MIXER_ADDR		0x4
#define MIXER_DATA		0x5

#define VOICE_VOLUME_LEFT  0x32					 
#define VOICE_VOLUME_RIGHT 0x33

/* soundblaster types */
#define SB_1			1
#define SB_2			2
#define SB_PRO		3
#define SB_16			4

/*
 * User type definitions.
 */

/*					 
 * Structure definitions.
 */

/*
 * Functions prototypes.
 */

/***** Non exported functions *****/
static void GetEnvSettings(void);
static uint16 HexWord(const char *pcHexStr);
static uint16 DspReset(void);
static void DspWrite(uint8 bData);
static uint8 DspRead(void);


/*
 * Global variables.
 */

/** Global Sound Blaster Variables **/
static uint16 wSbcBaseAddxG	=	0x220;				/* Sound Blaster base addr */
static uint16 wSbIrqG				=  0x05;				/* Sound Blaster IRQ */
static uint16 wSbLoDmaG			=  0x01;				/* Sound Blaster 8bit DMA channel */
static uint16 wSbHiDmaG			=  0x05;				/* Sound Blaster 16bit DMA channel */

static uint16 wCardG = SB_1;
static uint16 wMaxChannelsG = 0;
static uint16 wMaxBitsG = 0;
static uint16 wTransferCountG;			 /* block size transfered */
static uint16 wRecTransferCountG;		 /* block size transfered when recording */			
static uint8  bOutputFilterG;

static volatile uint8 bInterruptG;

/***********************************************
*
* uint16 SBlasterSfxInit(uint16 *pwIrq,uint16 *pwChnls,uint16 *pwBits)
*
* Parameters: Card capabilities
*	uint16 *pwIrq			: Soundblaster IRQ setting
* uint16 *pwChnls		: Max number of channels (1 or 2)
* uint16 *pwBits		: Max number of bits (8 or 16)
*
************************************************
*
* Resets and determines the type of SoundBlaster
* installed. Returns the capability of the card
*
************************************************
*
* Called by: SfxInit() in Sfx.c
*
* Depends on: nothing
*
***********************************************/
uint16 SBlasterSfxInit(uint16 *pwIrq,uint16 *pwChnls,uint16 *pwBits)
{
	extern uint16 wSbIrqG;
	extern uint16 wCardG;
	extern uint16 wMaxChannelsG;
	extern uint16 wMaxBitsG;
	uint16 wCount;
	uint16 wFail;
	uint8 bMajor,bMinor;

	GetEnvSettings();					/* Read BLASTER environment data */


 	/*
 	 * reset sound card 
 	 */
	wCount = 8;		/* 8 attempts...*/
	wFail = 1;
	while(wCount && wFail)
	{
		wFail = DspReset();
		wCount--;
	}
							
	if(wFail)
	{
		return 1;
	}

	/** get DSP version **/
	DspWrite(0xe1);
  bMajor = DspRead();
	bMinor = DspRead();

	*pwIrq = wSbIrqG;					/* SoundBlaster IRQ number */

/*
 * DEBUGGING / TESTING ONLY 
 */
#if EIGHT_BIT_TEST > 0
	printf("Testing DSP type: %d\n",EIGHT_BIT_TEST);
	bMajor = EIGHT_BIT_TEST;			/* pretend to be an earlier card */
#endif

	/*
	 * determine card capabilities 
	 */
	if(bMajor < 2)
	{
		/* Sound Blaster 1.5 or SBMVC installed */
		//printf("Sound Blaster detected\n");
		wCardG = SB_1;
		wMaxChannelsG = 1;
		wMaxBitsG = 8;
	}
	else if(bMajor == 2)
	{
		/* Sound Blaster 2 installed */
		//printf("Sound Blaster 2 detected\n");
		wCardG = SB_2;
		wMaxChannelsG = 1;
		wMaxBitsG = 8;
	}
	else if(bMajor == 3)
	{
		/* Sound Blaster Pro installed */
		//printf("Sound Blaster Pro detected\n");
		wCardG = SB_PRO;
		wMaxChannelsG = 2;	 
		wMaxBitsG = 8;
	}
	else if(bMajor >= 4)
	{
		/* Sound Blaster 16 or better installed */
		//printf("Sound Blaster 16 detected\n");
		wCardG = SB_16;
		wMaxChannelsG = 2;
	  wMaxBitsG = 16;

		/****
		printf("emulating SB2\n");
		wCardG = SB_2;
		wMaxChannelsG = 1;
		wMaxBitsG = 8;
		****/
	}

	/** return capabilities **/
	*pwChnls = wMaxChannelsG;
	*pwBits = wMaxBitsG;

	return 0;
}

/*******************************************************
*
* void SBlasterSfxStart(uint16 wSampleRate,uint16 wChannels,
												uint32 dwStartAddr,uint16 wBlockSize)
*
* uint16 wSampleRate :	output sample rate in Hertz
* uint16 wChannels 	 :  1 for mono, 2 for stereo
* uint32 dwStartAddr :  start address for DMA transfer
* uint16 wBlockSize  :  number of SAMPLES (not bytes) to be transfered
*
********************************************************
*
* Sets the sample rate and kicks off transfer
*
********************************************************
*
* Called by SfxInit() in Sfx.c
* 
* Depends on: SBlasterSfxInit() must be called first
*
*******************************************************/
void SBlasterSfxStart(uint16 wSampleRate,uint16 wChannels,
											uint32 dwStartAddr,uint16 wBlockSize)
{
	extern uint16 wCardG;
	extern uint16 wSbLoDmaG;
	extern uint16 wSbHiDmaG;
	extern uint8 bOutputFilterG;
	extern uint16 wTransferCountG;
	extern volatile uint8 bInterruptG;
	uint16 wTConst;

	DspWrite(0xD1);				/* Turn on output */

	/*
	 * Set sample rate
	 */
	if(wCardG == SB_16)
	{
		/* 
		 * DSP 4+, set rate directly 
		 */
		DspWrite(0x41);
		DspWrite(wSampleRate>>8);
		DspWrite(wSampleRate);
	}
	else
	{
		/* 
		 * Earlier Sound Blaster card
		 * calculate time constant for sample rate 
		 */
		wTConst = 65536 - (256000000/(wChannels*wSampleRate));
		DspWrite(0x40);
		DspWrite((uint8)(wTConst>>8));
	}

	/*											 
	 * setup DSP 
	 */
	switch(wCardG)
	{
		case SB_16:
			/*
		 	 *  Set for 16 bit auto-init transfer 
		   */
			if(wSbHiDmaG > 3)
				HiDmaReadSet(dwStartAddr,wBlockSize*4,wSbHiDmaG,AUTO);		/* Use hi DMA channel */
			else
				LoDmaReadSet(dwStartAddr,wBlockSize*4,wSbLoDmaG,AUTO);		/* Use lo DMA channel */

			DspWrite(0xb6);			/* 16 bit output */
			if(wChannels == 2)
			{
				DspWrite(0x30);		/* 16 bit stereo */
			}
			else
			{
				DspWrite(0x10);		/* 16 bit mono */
			}
			break;
		case SB_PRO:
		{
			uint8 bTmp;																		 

			if(wChannels == 2)
			{
				/* 
				 * Set stereo flag in mixer
				 */
				outp(SBCPort(MIXER_ADDR),0xE);
				bTmp = inp(SBCPort(MIXER_DATA));
				outp(SBCPort(MIXER_DATA),(bTmp | 0x02));
			}

			/*
			 * To get a Pro card to work in stereo properly, it
			 * should be first setup to transfer a single data 
			 * byte in single cycle mode, then set to auto-init mode
			 * as normal.This fixes the stereo so that
			 * left data comes out of the left channel
			 * and right from the right. However, I can't get this
			 * to work.
			 * Instead, a stereo reverse flag is set in Sfx.c
			 * which repans sounds to the opposite channel.
			 */

			LoDmaReadSet(dwStartAddr,wBlockSize*2,wSbLoDmaG,AUTO);		
		
			/*
			 * Preserve filter status then turn off low pass filter
			 */
			outp(SBCPort(MIXER_ADDR),0xE);
			bOutputFilterG = inp(SBCPort(MIXER_DATA));
			outp(SBCPort(MIXER_DATA),(bOutputFilterG | 0x20));
			
			DspWrite(0x48);	

			break;
		}
	  case SB_2:
	 		/*
		 	 * Set for 8 bit mono auto-init transfer 
			 */
			LoDmaReadSet(dwStartAddr,wBlockSize*2,wSbLoDmaG,AUTO);
			DspWrite(0x48);
			break;
		case SB_1:
			/*
		   * Single Cycle 8 bit	transfer
			 * However, if DMA set to auto-init it'll work
			 * OK. Then only the DSP needs resetting in the
			 * interrupt.
		   */
			LoDmaReadSet(dwStartAddr,wBlockSize*2,wSbLoDmaG,AUTO);
			DspWrite(0x14);
			break;
	}

	/*
	 * Write block size to transfer 
	 */
	wBlockSize--;
	wTransferCountG = wBlockSize;	/* store block size for single cycle refresh */
	DspWrite(wBlockSize);				/* write lo byte of size */
	DspWrite(wBlockSize>>8);		/* write hi byte of size */

	/*
	 * Write command byte if needed 
	 */
	switch(wCardG)
	{
		case SB_PRO:
				DspWrite(0x90);
			break;
		case SB_2:
			DspWrite(0x1C);
			break;
	}
}

void SBlasterRecStart(uint16 wSampleRate,uint32 dwStartAddr,uint16 wBlockSize)
{
	uint16 wTConst;

	DspReset();

	/*
	 * Set the sample rate
	 */
	wTConst = 65536 - (256000000/wSampleRate);
	DspWrite(0x40);
	DspWrite((uint8)(wTConst>>8));

	/*
	 * Single cycle 8 bit transfer
	 * with DMA set to auto-init.
	 * DSP needs resetting in the interrupt
	 * This should work with all SoundBlasters
	 */
	LoDmaWriteSet(dwStartAddr,wBlockSize*2,wSbLoDmaG,AUTO);
	wBlockSize--;
	wRecTransferCountG = wBlockSize;
	DspWrite(0x24);		
	DspWrite((uint8)wRecTransferCountG);
	DspWrite((uint8)(wRecTransferCountG>>8));
}



/**********************************************
*
* void SBlasterAckInt(void) 
*
***********************************************
*
* Acknowledges a DSP interrupt with the
* SoundBlaster DSP. Also resets any data transfer
* for single cycle mode
*
***********************************************
*
* Called by: DSP interrupt handler in sfx.c
*
* Depends on: SBlasterSfxInit() must be called first
*
**********************************************/
void SBlasterAckInt(void)
{
	extern uint16 wCardG;
	extern uint16 wTransferCountG;
	extern volatile uint8 bInterruptG;

	switch(wCardG)
	{
		case SB_16:
 			inp(SBCPort(0x0F));			/* 16 bit acknowledgement */
			break;
		case SB_PRO:
			bInterruptG = 1;			/* set interrupt flag  (SB-PRO only) */
		case SB_2:
			inp(SBCPort(0x0E));		/* 8 bit acknowledgement */
			break;
		case SB_1:
			/*
			 * Restart single cycle transfer 
			 */
			
			/*
			 * DMA transfer would normally be started here
			 * but is working on auto-init
			 */

			DspWrite(0x14);
			DspWrite((uint8)wTransferCountG);
			DspWrite((uint8)(wTransferCountG>>8));

			inp(SBCPort(0x0E));		/* 8 bit acknowledgement */
			break;
	}		
}

void SBlasterAckRecInt(void)
{
	extern uint16 wTransferCountG;

	/*
	 * Acknowledge record interrupt
	 */
	DspWrite(0x24);		
	DspWrite((uint8)wRecTransferCountG);
	DspWrite((uint8)(wRecTransferCountG>>8));

	inp(SBCPort(0x0E));			/* 8 bit acknowledgement */
}

/****************************************************
*
* void SBlasterSfxSetVolume(uint8 bVolume)
*
* uint8 bVolume: voice output volume level
*
*****************************************************
*
* Sets the left and right volume level for the
* soundblaster card 
*
****************************************************
*
* Called by: Sfx.c
*
* Depends on: SBlasterSfxInit() must be called first
*
***************************************************/
void SBlasterSfxSetVolume(uint8 bVolume)
{
	extern uint16 wCardG;

	switch(wCardG)
	{
		case SB_2:
			outp(SBCPort(MIXER_ADDR),0x0A);
			outp(SBCPort(MIXER_DATA),bVolume>>5);
			break;
		case SB_PRO:
		{
			uint8 bBitMap;

			bBitMap = bVolume;
			bBitMap &= 0xF0;
			bBitMap += bVolume>>4;

			outp(SBCPort(MIXER_ADDR),0x04);
			outp(SBCPort(MIXER_DATA),bBitMap);
			break;
		}
		case SB_16:
			outp(SBCPort(MIXER_ADDR),VOICE_VOLUME_LEFT);
			outp(SBCPort(MIXER_DATA),bVolume);
			outp(SBCPort(MIXER_ADDR),VOICE_VOLUME_RIGHT);
			outp(SBCPort(MIXER_DATA),bVolume);
			break;
	}
}

/*************************************************
*
* void SBlasterSfxUnInit(void)															 
* 
**************************************************
*
* Uninitialises the SoundBlaster, stopping
* any DMA transfer
*
**************************************************
*
* Called by: SfxUnInit() in Sfx.c
*
* Depends on: SBlasterSfxInit() must be called first
*
*************************************************/
void SBlasterSfxUnInit(void)															 
{
	extern uint16 wCardG;
	extern uint8 bOutputFilterG;
	uint8 bTmp;

	switch(wCardG)
	{
		case SB_16:
			DspWrite(0xd9);		/* exit 16bit auto init */
			break;
		case SB_PRO:
			DspReset();

			/*
			 * Restore filter
			 */
			outp(SBCPort(MIXER_ADDR),0xE);
			outp(SBCPort(MIXER_DATA),bOutputFilterG);
			
		  /*
			 * Set to mono mode
			 */
			outp(SBCPort(MIXER_ADDR),0xE);
			bTmp = inp(SBCPort(MIXER_DATA));
			outp(SBCPort(MIXER_DATA),(bTmp & 0xFD));

			DspReset();
			DspReset();

			break;
		case SB_2:
			DspWrite(0xDA);		/* exit 8bit auto init */
			break;
	}
}

/************* Non exported SB16 functions **********/

/************************************************
*
* void GetEnvSettings(void)
*
*************************************************
*
* Reads the BLASTER environment variable to
* configure the base address, DMA channel and IRQ
*
*************************************************
*
* Called by: SBlasterSfxInit()
*
* Depends on: Autoexec.bat must include correct
*             BLASTER environment line
*
************************************************/
void GetEnvSettings(void)
{
	extern uint16 wSbcBaseAddxG;
	extern uint16 wSbIrqG;
	extern uint16 wSbLoDmaG;
	extern uint16 wSbHiDmaG;
	const char *pcBlaster;

	pcBlaster = getenv("BLASTER");

  /** scan environment string **/
	while(*pcBlaster)
	{

		/** Look for cards base address **/
		if(*pcBlaster == 'a' || *pcBlaster == 'A')
		{
			pcBlaster++;

			if(pcBlaster)
			{
				if(pcBlaster == ':')
					pcBlaster++;
				
				/** update address **/
				wSbcBaseAddxG = HexWord(pcBlaster);
			}
		}

		/** Look for IRQ number **/
		if(*pcBlaster == 'i' || *pcBlaster == 'I')
		{
			pcBlaster++;

			if(pcBlaster)
			{
				if(pcBlaster == ':')
					pcBlaster++;

				/** update number **/
				wSbIrqG = HexWord(pcBlaster);
			}
		}

		/** Look for low DMA channel **/
		if(*pcBlaster == 'd' || *pcBlaster == 'D')
		{
			pcBlaster++;

			if(pcBlaster)
			{
				if(pcBlaster == ':')
					pcBlaster++;

				/** update channel **/
				wSbLoDmaG = HexWord(pcBlaster);
			}
		}

		/** Look for high DMA channel **/
		if(*pcBlaster == 'h' || *pcBlaster == 'H')
		{
			pcBlaster++;

			if(pcBlaster)
			{
				if(pcBlaster == ':')
					pcBlaster++;

				/** update hi channel **/
				wSbHiDmaG = HexWord(pcBlaster);
			}
		}

		while(*pcBlaster && *pcBlaster != ' ') 
			pcBlaster++;
	  while(*pcBlaster == ' ')
			pcBlaster++;
	}
}

/******************************************
*
* uint16 HexWord(const char *pcHexStr)
*
* const char *pcHexStr: string of hexadecimal characters
*
* returns: uint16 : numerical value of string
*
*******************************************
*
* Converts a string of hex characters into a numerical
* value.
*
*******************************************
*
* Called by: GetEnvSettings()
*
* Depends on: nothing
*
******************************************/
uint16 HexWord(const char *pcHexStr)
{
	uint16	wValue = 0;

	while(*pcHexStr)
	{
		if( *pcHexStr >= '0' && *pcHexStr <= '9')      
			wValue = wValue*16 + (*pcHexStr - '0');
		else if (*pcHexStr >= 'a' && *pcHexStr <= 'f')
			wValue = wValue*16 + (*pcHexStr - 'a' + 10);
		else if (*pcHexStr >= 'A' && *pcHexStr <= 'F')
			wValue = wValue*16 + (*pcHexStr - 'A' + 10);
		else
			break;
			
		pcHexStr++;	/** increment character pointer **/	
	} 

	return wValue;
}

										
/***********************************************
*
* uint16 DspReset(void)
* 
* returns: uint16 : error flag: 1=error 0=OK
*
************************************************
*
* Resest the SoundBlasters DSP chip
*
************************************************
*
* Called by: SBlasterSfxInit()
*
* Depends on: nothing
*
***********************************************/
uint16 DspReset(void)
{
	volatile uint16 wCount;
	uint16 wAcknowledge;

	/** Reset Digital Sound Processor **/

	/** Write 1 to Reset port **/
	outp(SBCPort(RESET_PORT),1);

	/** wait > 3 microseconds **/
	for(wCount=0; wCount<0x2000; wCount++);						

	/** Write 0 to Reset port **/
	outp(SBCPort(RESET_PORT),0);

	/** Poll for 0xAA from Read Data Port **/
	wCount = 0x2000;
	while(wCount && !(inp(SBCPort(READ_STATUS)) & 0x80))		/* wait for data ready */
		wCount--;
	wAcknowledge = inp(SBCPort(READ_DATA));

	/** Check acknowledgement and return **/
	if((wAcknowledge & 0xFF) != 0xAA || wCount == 0)
		return 1;			/* failed to reset! */
	else
		return 0;			/* reset ok */
}

/*********************************************
*
* void DspWrite(uint8 bData)
*
* uint8 bData : byte to write to DSP chip
*
**********************************************
*
* Writes a byte to the DSP
*
**********************************************
*
* Called by: Sound Blaster functions
*
* Depends on: DspReset() must have been called first
*
********************************************/
void DspWrite(uint8 bData)
{
	/** wait for port to become ready **/
	while(inp(SBCPort(WRITE_STATUS)) & 0x80);	

	/** write data to port **/
	outp(SBCPort(WRITE_DATA),bData);
}

/*********************************************
*
* uint8 DspRead(void)
*
* returns: uint8 : byte read from DSP
*
**********************************************
*																								
* Reads a byte from the DSP port
*
**********************************************
*
* Called by: Sound Blaster functions
*
* Depends on: DspReset() must have been called
*
*********************************************/
uint8 DspRead(void)
{
	/** wait until data is ready **/
	while(!(inp(SBCPort(READ_STATUS)) & 0x80));

	/** read data from port **/
  return inp(SBCPort(READ_DATA));
}
	
