/******************************************************
*
*	File: sfxdma.c
*
* Contains functions for setting DMA data transfer 
* from memory to I/O.
*
*******************************************************
*
* $Log:   P:/ridger/ridger/SFXDMA.C_v  $
 * 
 *    Rev 1.0   13 Jul 1995 14:39:46   DaveS
 * Put Back after we stopped being naughty children
 * 
 *    Rev 1.0   18 Nov 1994 15:24:24   AndrewC
 * Initial revision.
*
******************************************************/

/*
 * Includes
 */
#include "c:\watcom\h\stdio.h"
#include "c:\watcom\h\stdlib.h"
#include <conio.h>
#include <dos.h>

#include "utypes.h"
#include "sfxdma.h"

/*
 * #defines and macros
 */
/** DMA ports **/
#define DMA_CMD_REG 	0x08
#define DMA_MASK_REG  0x0A
#define DMA_MODE_REG	0x0B
#define DMA_FLIP_FLOP 0x0C
#define DMA_DISABLE		0x0D
#define DMA_MASK_CLR	0x0E
													 
/** DMA2 ports **/
#define DMA2_BASE			0xc0
#define DMA2_CMD_REG 	0xd0
#define DMA2_MASK_REG 0xd4
#define DMA2_MODE_REG 0xd6
#define DMA2_FLP_FLOP 0xd8
#define DMA2_DISABLE  0xda
#define DMA2_MASK_CLR 0xdc


/*
 *  inline assembler functions to switch interrupts 
 */
#pragma aux InterruptsOff = \
 						"cli";
 
#pragma aux InterruptsOn = \
						"sti";

/*
 * function prototypes
 */
void InterruptsOff(void);
void InterruptsOn(void);


/*
 * Global variables
 */

/** pageTable maps channel no. to i/o port of DMA page register **/
static uint16 wLoPageTable[4] = {0x87,0x83,0x81,0x82};
static uint16 wHiPageTable[4] = {0x8f,0x8b,0x89,0x8a};


/****************************************************
*
* void LoDmaReadSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
*
* uint32 dwStart: Start address of data for DMA transfer
* uint16 wLen: 	  Number of BYTES to transfer, max 64k
* uint16 wChnnl:  DMA channel to use 0-3 valid
* uint16 wAuto:		0=Single Cycle 1= Auto Initialise
*
*****************************************************
*
* Sets up the DMA controller for auto-initialised data 
* transfer across a low DMA channel.
*
*****************************************************
*
* Called by: SfxInit()
*
* Depends on: start address must be in low (DOS) memory
*
****************************************************/
void LoDmaReadSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
{	
	uint8 bMode;
	uint8 bPage;
	uint16 wPort;

	wLen--;

	outp(DMA_DISABLE,1);	/* Reset low DMA channel */
	outp(DMA_MASK_CLR,1);	/* Clear mask register */
	outp(DMA2_MASK_CLR,1);	/* Clear mask register on DMA-2 */

	/** build DMA read command **/

	if(wAuto == AUTO)
		bMode = 0x18;		/* demand mode | Increment | AutoInit on |Read transfer */
	else
		bMode = 0x08;		/* demand mode | Increment | Read transfer */
	bMode += wChnnl;

	/** build port address **/
	wPort = wChnnl << 1;		

	/** get 4 bit page **/
	bPage = (uint8)(dwStart>>16);

	InterruptsOff();

	/** enable DMA controller **/
	outp(DMA_CMD_REG,0);				/* clear command register */

	/** write page address **/	
	outp(wLoPageTable[wChnnl],bPage);		/* get port from pageTable */

	/** write LSW of start address **/
	outp(DMA_FLIP_FLOP,0);					/* reset flip flop */
	outp(wPort,(uint8)dwStart);			  /* write LSB */
	outp(wPort,(uint8)(dwStart>>8));		/* write MSB */			

	/** write length to transfer **/
	outp(DMA_FLIP_FLOP,0);					/* reset flip flop */
	wPort++;												/* get length port */
	outp(wPort,(uint8)wLen);					/* write LSB */
	outp(wPort,(uint8)(wLen>>8));		/* write MSB */

	/** set transfer mode **/
	outp(DMA_MODE_REG,bMode);				/* write mode byte */

	/** set mask **/
	outp(DMA_MASK_REG,wChnnl);				

	InterruptsOn();
}


/****************************************************
*
* void HiDmaReadSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
*
* uint32 dwStart: Start address of data for DMA transfer 
* uint16 wLen: 	  Number of BYTES to transfer, max 128k
* uint16 wChnnl:  DMA channel to use 5-7 valid
* uint16 wAuto:   0=Single Cycle 1=Auto Initialise
*
*****************************************************
*
* Sets up the DMA controller for auto-initialised 
* data transfer across a high (16bit) DMA channel.
*
*****************************************************
*
* Called by: SfxInit()
*
* Depends on: start address must be in low (DOS) memory
*
****************************************************/
void HiDmaReadSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
{
	uint8 bMode;
	uint8 bPage;
	uint16 wPort;

	wChnnl -= 4;		/* convert channel number for DMA 2 */
	
	wLen--;					/* adjust length for DMA */

	/*
	 * Reset DMA controllers 
	 */
	outp(DMA2_DISABLE,1);		/* Reset Hi DMA */
	outp(DMA_DISABLE,1);		/* Reset Lo DMA */
	outp(DMA2_MASK_CLR,1);	/* Clear Hi Mask */
	outp(DMA_MASK_CLR,1);		/* Clear Lo Mask */

 	/** build DMA read command **/
	if(wAuto == AUTO)
		bMode = 0x18;		/* demand mode | Increment | AutoInit On |Read transfer */
	else
		bMode = 0x08;		/* demand mode | Increment | Read transfer */
	bMode += wChnnl;

	/** build port address **/
	wPort = DMA2_BASE + (wChnnl << 2);		

	/** get 4 bit page **/
	bPage = (uint8)(dwStart>>16);

 	InterruptsOff();

	/** enable DMA controller **/
	outp(DMA2_CMD_REG,0);				/* clear command register */

	/** write page address **/	
	outp(wHiPageTable[wChnnl],bPage);		/* get port from pageTable */

	/** divide address and length by two for WORDS instead of BYTES **/
	dwStart >>= 1; 
	wLen >>= 1;

	/** write LSW of start address **/
	outp(DMA2_FLP_FLOP,0);						/* reset flip flop */
	outp(wPort,(uint8)dwStart);				/* write LSB */
	outp(wPort,(uint8)(dwStart>>8));		/* write MSB */			

	/** write length to transfer **/
	outp(DMA2_FLP_FLOP,0);					/* reset flip flop */
	wPort += 2;											/* get length port */
	outp(wPort,(uint8)wLen);					/* write LSB */
	outp(wPort,(uint8)(wLen>>8));		/* write MSB */

	/** set transfer mode **/
	outp(DMA2_MODE_REG,bMode);				/* write mode byte */

	/** set mask **/
	outp(DMA2_MASK_REG,wChnnl);				/* enable DMA channel */

	InterruptsOn();
}				

/****************************************************
*
* void LoDmaWriteSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
*
* uint32 dwStart: Start address of data for DMA transfer
* uint16 wLen: 	  Number of BYTES to transfer, max 64k
* uint16 wChnnl:  DMA channel to use 0-3 valid
* uint16 wAuto:		0=Single Cycle 1= Auto Initialise
*
*****************************************************
*
* Sets up the DMA controller for auto-initialised data 
* transfer across a low DMA channel.
*
*****************************************************
*
* Called by: SfxInit()
*
* Depends on: start address must be in low (DOS) memory
*
****************************************************/
void LoDmaWriteSet(uint32 dwStart,uint16 wLen,uint16 wChnnl,uint16 wAuto)
{
	uint8 bMode;
	uint8 bPage;
	uint16 wPort;

	wLen--;

	outp(DMA_DISABLE,1);	/* Reset low DMA channel */
	outp(DMA_MASK_CLR,1);	/* Clear mask register */
	outp(DMA2_MASK_CLR,1);	/* Clear mask register on DMA-2 */

	/** build DMA read command **/

	if(wAuto == AUTO)
		bMode = 0x14;		/* demand mode | Increment | AutoInit on |Write transfer */
	else
		bMode = 0x04;		/* demand mode | Increment | Write transfer */
	bMode += wChnnl;

	/** build port address **/
	wPort = wChnnl << 1;		

	/** get 4 bit page **/
	bPage = (uint8)(dwStart>>16);

	InterruptsOff();

	/** enable DMA controller **/
	outp(DMA_CMD_REG,0);				/* clear command register */

	/** write page address **/	
	outp(wLoPageTable[wChnnl],bPage);		/* get port from pageTable */

	/** write LSW of start address **/
	outp(DMA_FLIP_FLOP,0);					/* reset flip flop */
	outp(wPort,(uint8)dwStart);			  /* write LSB */
	outp(wPort,(uint8)(dwStart>>8));		/* write MSB */			

	/** write length to transfer **/
	outp(DMA_FLIP_FLOP,0);					/* reset flip flop */
	wPort++;												/* get length port */
	outp(wPort,(uint8)wLen);					/* write LSB */
	outp(wPort,(uint8)(wLen>>8));		/* write MSB */

	/** set transfer mode **/
	outp(DMA_MODE_REG,bMode);				/* write mode byte */

	/** set mask **/
	outp(DMA_MASK_REG,wChnnl);				

	InterruptsOn();
}


/*****************************************************
*
* void *DosMemAlloc(uint32 dwSize,uint32 *pdwSelector)
*
* uint32 dwSize: number of bytes to allocate
* uint32 pdwSelector: selector of allocated memory
*
* returns: pointer to allocated memory
*					 zero if allocation failed.
*
******************************************************
*
*	Allocates a portion of DOS memory from protected mode.
*
******************************************************
*
* Called by: RecursiveAlloc()
*
* Depends on: nothing
*
*****************************************************/
void *DosMemAlloc(uint32 dwSize, uint32 *pdwSelector)
{
	union REGS r;

	r.x.eax = 0x0100;
	r.x.ebx = (dwSize + 15) >> 4;

	int386(0x31,&r,&r);

	*pdwSelector = r.x.edx;		/* selector of allocated memory */

	if(r.x.cflag) /* failed */
		return ((uint32) 0);

	return (void *)((r.x.eax & 0xffff) << 4);
}			  


/*******************************************************
*
* uint16 DosMemFree(uint32 dwSelector)
*
********************************************************
*
* Frees memory allocated in low (DOS) memory
*
********************************************************
*
* Called by: SfxUnInit()
*
* Depends on: Memory must have been allocated by DosMemAlloc()
*
*******************************************************/
uint16 DosMemFree(uint32 dwSelector)
{
	union REGS r;

	r.x.eax = 0x0101;
	r.x.edx = dwSelector; /* selector of block to be freed */

	int386(0x31,&r,&r);

	if(r.x.cflag)	 /* failed */
		return 1;

	return 0;
}

/*****************************************************
*
*	void *RecursiveAlloc(uint32 dwSize,uint32 *pdwSelector)
*
* uint32 dwSize : Size of memory to allocate (64k max)
* uint32 *pdwSelector : Selector of allocated memory
*
* Returns : 20bit pointer to memory location
*
******************************************************
*
* Allocates upto 64k of DOS memory from protocted mode.
*	Memory will not straddle a page boundary.
* 
* Note: This function recursively calles itself until
* the allocated memory is contained in one page.
*
******************************************************
*
* Called by: SfxInit()
*
* Depends on: 
*
* ***************************************************/
void *RecursiveAlloc(uint32 dwSize,uint32 *pdwSelector)
{
	void *pDosPtr;
	void *pNewPtr;
	uint32 dwDosSlct;
	uint32 dwNewSlct;

	pDosPtr = DosMemAlloc(dwSize,&dwDosSlct);

	if(( (uint32)pDosPtr >>16) != ( ( ((uint32)pDosPtr) + dwSize) >>16) )
	{
		/** if memory is across boundary, try again **/
		pNewPtr = RecursiveAlloc(dwSize,&dwNewSlct);		/** RECURSION **/

		/** Deallocate unused memory **/
		DosMemFree(dwDosSlct);

		*pdwSelector = dwNewSlct;
		return pNewPtr;
	}
	else
	{
		/** memory allocation OK, return pointer and selector **/
		*pdwSelector = dwDosSlct;
		return pDosPtr;
	}
}
