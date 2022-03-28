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
#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to u_char */
#define itob(i)		((i)/10*16 + (i)%10)		/* u_char to BCD */

#define CdSeekL(p)	CdControl(CdlSeekL, (u_char *)p, 0)
#define CdSeekP(p)	CdControl(CdlSeekP, (u_char *)p, 0)
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
	unsigned char minute;
	unsigned char second;
	unsigned char sector;
	unsigned char track;
};
typedef struct CdlPos CdlLOC;

/*
 *	Attenuator
 */
struct ATV {
	unsigned char	val0;
	unsigned char	val1;
	unsigned char	val2;
	unsigned char	val3;
};
typedef struct ATV CdlATV;

/*
 *	File System
 */
typedef struct {
	CdlLOC	pos;
	u_long	size;
	char	name[/*12*/16];
} CdlFILE;


/*
 *	Streaming Structures
 */
typedef struct {
	u_short	id;			/* always 0x0x0160 */
	u_short	type;			
	u_short	secCount;	
	u_short	nSectors;
	u_long	frameCount;
	u_long	frameSize;
	
	u_short	width;
	u_short	height;
	
	u_long	headm;
	u_long	headv;
} StSECTOR;				/* CD-ROM STR ç\ë¢ëÃ */

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
int	CdSetDebug(int);
char	*CdComstr(u_char);
char	*CdIntstr(u_char);
int	CdGetToc2(int, CdlLOC *);
int	CdControl(u_char, u_char *, u_char *);
int	CdControlB(u_char, u_char *, u_char *);
int	CdControlF(u_char, u_char *);
int	CdSync(int, u_char *);
int	CdGetSector(void *, int);
int	CdRead(int, u_long *, int);
int	CdSetMode(long);
int	CdRead2(long);
int	CdSetFilter(u_char, u_char);
int	CdMix(CdlATV *);
u_long	CdSyncCallback(void (*func)());
u_long	CdReadyCallback(void (*func)());
CdlFILE *CdSearchFile(CdlFILE *, char *);
CdlLOC	*CdIntToPos(int, CdlLOC *);
int	CdPosToIntw(CdlLOC *);

/*
 *	Prototypes for Streaming
 */
void	StSetRing(u_long *ring_addr,u_long ring_size);
void	StClearRing(void);
void	StUnSetRing(void);
u_long	StFreeRing();
void	StSetStream(u_long mode,u_long start_frame,u_long end_frame,
		    int (*func1)(),int (*func2)());
void	StSetEmulate(u_long *addr,u_long mode,u_long start_frame,
		     u_long end_frame,int (*func1)(),int (*func2)());
u_long	StFreeRing(u_long *base);
u_long	StGetNext(u_long **addr,u_long **header);
void	StSetMask(u_long mask,u_long start,u_long end);
void	StCdInterrupt(void);
