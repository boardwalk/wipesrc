/*
 *  (C) Copyright 1993/1994 Sony Computer Entertainment ,Tokyo,Japan.
 *                      All Rights Reserved
 *
 *              libcd.h: CD-ROM sub system hendler
 *
 *$Id: libcd.h,v 1.13 1994/10/21 03:46:45 suzu Exp $
 *
 * CD-ROM Primitive Command list:
 *
 *	Symbol		type	Contents
 *	------------------------------------------------------
 *	CdlNop		B	NOP
 *	CdlSetloc	B	Set position
 *	CdlPlay		B	CD-DA Play
 *	CdlForward	B	Forward
 *	CdlBackword	B	Backword
 *	CdlReadN	B	Read with retry
 *	CdlStanby	N	Standby
 *	CdlStop		N	Stop
 *	CdlPause	N	Pause
 *	CdlMute		B	Mute on
 *	CdlDemute	B	Mute off
 *	CdlBetfilter	B	Set SubHeader filter
 *	CdlSetmode	B	Set mode
 *	CdlGetlocL	B	Get logical position
 *	CdlGetlocP	B	Get phisycal position
 *	CdlSeekL	N	Logical Seek
 *	CdlSeekP	N	Phisical Seek
 *	CdlReadS	B	Read without retry
 *	CdlReset	B	Reset
 *	------------------------------------------------------
 *			B: Blocking, N: Non-Blocking operation
 *
 *
 *	Symbol		arg		result
 *	--------------------------------------------------------------
 *	CdlNop		-		status
 *	CdlSetloc	min,sec,sector	status
 *	CdlPlay       	-		status
 *	CdlForward	-		status
 *	CdlBackword	-		status
 *	CdlReadN	-		status
 *	CdlStanby	-		status
 *	CdlStop		-		status
 *	CdlPause	-		status
 *	CdlMute		-		status
 *	CdlDemute	-		status
 *	CdlSetfilter	file,chan	status
 *	CdlSetmode	mode		status
 *	CdlGetlocL	-		min,sec,sector,mode,file, chan
 *	CdlGetlocP	-		track,index,min,sec,frame,
 *							amin,asec,aframe
 *	CdlSeekL	-		status
 *	CdlSeekP	-		status
 *	CdlReadS	-		status
 *	CdlReset	-		status
 *	--------------------------------------------------------------
 */
#ifndef LIBCD_H
#define LIBCD_H

/*
 * CD-ROM Basic System
 */

/*
 * CD-ROM Mode
 */
#define CdlModeSpeed	0x80	/* 0: normal speed	1: double speed	*/
#define CdlModeRT	0x40	/* 0: ADPCM off		1: ADPCM on	*/
#define CdlModeSize1	0x20	/* 0: 2048 byte		1: 2340byte	*/
#define CdlModeSize0	0x10	/* 0: -			1: 2328byte	*/
#define CdlModeSF	0x08	/* 0: Channel off	1: Channel on	*/
#define CdlModeRept  	0x04	/* 0: Report off	1: Report on	*/
#define CdlModeAP	0x02	/* 0: AutoPause off	1: AutoPause on */
#define CdlModeDA	0x01	/* 0: CD-DA off		1: CD-DA on	*/

/*
 * CD-ROM Primitive Commands
 */
#define CdlNop		0x01
#define CdlSetloc	0x02
#define CdlPlay		0x03
#define CdlForward	0x04
#define CdlBackward	0x05
#define CdlReadN	0x06
#define CdlStandby	0x07
#define CdlStop		0x08
#define CdlPause	0x09
#define CdlReset	0x0a
#define CdlMute		0x0b
#define CdlDemute	0x0c
#define CdlSetfilter	0x0d
#define CdlSetmode	0x0e
#define CdlGetparam	0x0f
#define CdlGetlocL	0x10
#define CdlGetlocP	0x11
#define CdlReadT	0x12
#define CdlGetTN	0x13
#define CdlGetTD	0x14
#define CdlSeekL	0x15
#define CdlSeekP	0x16
#define CdlReadS	0x1B

/*
 * Interrupts
 */
#define CdlNoIntr	0x00	/* No interrupt */
#define CdlDataReady	0x01	/* Data Ready */
#define CdlComplete	0x02	/* Command Complete */
#define CdlAcknowledge	0x03	/* Acknowledge (reserved) */
#define CdlDataEnd	0x04	/* End of Data Detected */
#define CdlDiskError	0x05	/* Error Detected */

/*
 * Library Macros
 */
#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to uint8_t */
#define itob(i)		((i)/10*16 + (i)%10)		/* uint8_t to BCD */

#define CdSeekL(p)	CdControl(CdlSeekL, (uint8_t *)p, 0)
#define CdSeekP(p)	CdControl(CdlSeekP, (uint8_t *)p, 0)
#define CdGetToc(table)	CdGetToc2(1, table)
#define CdStandby()	CdControl(CdlStandby,  0, 0)
#define CdPause()	CdControl(CdlPause,    0, 0)
#define CdStop()	CdControl(CdlStop,     0, 0)
#define CdMute()	CdControl(CdlMute,     0, 0)
#define CdDeMute()	CdControl(CdlDemute,   0, 0)
#define CdForward()	CdControl(CdlForward,  0, 0)
#define CdBackward()	CdControl(CdlBackward, 0, 0)
#define CdPlay()	CdControl(CdlPlay,     0, 0)

/*
 *	High-Level Status
 */
#define CdlStbMute	0x20
#define CdlStbPause	0x10
#define CdlStbSM	0x08
#define CdlStbRead	0x04
#define CdlStbSeek	0x02
#define CdlStbBusy	0x01

/*
 *	Position
 */
#define CdlMAXTOC	100
struct CdlPos {
	uint8_t minute;
	uint8_t second;
	uint8_t sector;
	uint8_t track;
};
typedef struct CdlPos CdlLOC;

/*
 *	Attenuator
 */
struct ATV {
	uint8_t	val0;
	uint8_t	val1;
	uint8_t	val2;
	uint8_t	val3;
};
typedef struct ATV CdlATV;

/*
 *	File System
 */
typedef struct {
	CdlLOC	pos;
	uint32_t	size;
	char	name[/*12*/16];
} CdlFILE;


/*
 *	Streaming Structures
 */
typedef struct {
	uint16_t	id;			/* always 0x0x0160 */
	uint16_t	type;
	uint16_t	secCount;
	uint16_t	nSectors;
	uint32_t	frameCount;
	uint32_t	frameSize;

	uint16_t	width;
	uint16_t	height;

	uint32_t	headm;
	uint32_t	headv;
} StSECTOR;				/* CD-ROM STR �\���� */

#define  StFREE       0x0000
#define  StREWIND     0x0001
#define  StCOMPLETE   0x0002
#define  StBUSY       0x0003
#define  StLOCK       0x0004

#define  EDC 0
#define  SECTOR_SIZE (512)	/* Sector Size (word) */
#define  HEADER_SIZE (8)	/* Header Size (word) */

#define  StSTATUS         0x00
#define  StVER            0x00
#define  StTYPE           0x01
#define  StSECTOR_OFFSET  0x02
#define  StSECTOR_SIZE    0x03
#define  StFRAME_NO       0x04
#define  StFRAME_SIZE     0x06

#define  StMOVIE_WIDTH    0x08
#define  StMOVIE_HEIGHT   0x09
#define  StMOVIE_HEADM    0x0a
#define  StMOVIE_HEADV    0x0c

/*
 *	Prototypes
 */
void	CdInit();
int32_t	CdSetDebug(int32_t);
char	*CdComstr(uint8_t);
char	*CdIntstr(uint8_t);
int32_t	CdGetToc2(int32_t, CdlLOC *);
int32_t	CdControl(uint8_t, uint8_t *, uint8_t *);
int32_t	CdControlB(uint8_t, uint8_t *, uint8_t *);
int32_t	CdControlF(uint8_t, uint8_t *);
int32_t	CdSync(int32_t, uint8_t *);
int32_t	CdGetSector(void *, int32_t);
int32_t	CdRead(int32_t, uint32_t *, int32_t);
int32_t	CdSetMode(int32_t);
int32_t	CdRead2(int32_t);
int32_t	CdSetFilter(uint8_t, uint8_t);
int32_t	CdMix(CdlATV *);
uint32_t	CdSyncCallback(void (*func)());
uint32_t	CdReadyCallback(void (*func)());
CdlFILE *CdSearchFile(CdlFILE *, char *);
CdlLOC	*CdIntToPos(int32_t, CdlLOC *);
int32_t	CdPosToIntw(CdlLOC *);

/*
 *	Prototypes for Streaming
 */
void	StSetRing(uint32_t *ring_addr,uint32_t ring_size);
void	StClearRing();
void	StUnSetRing();
uint32_t	StFreeRing();
void	StSetStream(uint32_t mode,uint32_t start_frame,uint32_t end_frame,
		    int32_t (*func1)(),int32_t (*func2)());
void	StSetEmulate(uint32_t *addr,uint32_t mode,uint32_t start_frame,
		     uint32_t end_frame,int32_t (*func1)(),int32_t (*func2)());
uint32_t	StFreeRing(uint32_t *base);
uint32_t	StGetNext(uint32_t **addr,uint32_t **header);
void	StSetMask(uint32_t mask,uint32_t start,uint32_t end);
void	StCdInterrupt();

#endif