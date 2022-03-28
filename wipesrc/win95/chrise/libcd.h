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
 *	CdlNop			B	NOP		
 *	CdlSetloc		B	Set position	
 *	CdlPlay			B	CD-DA Play
 *	CdlForward		B	Forward
 *	CdlBackword		B	Backword
 *	CdlReadN	  	B	Read with retry
 *	CdlStanby		N	Standby
 *	CdlStop			N	Stop
 *	CdlPause	  	N	Pause
 *	CdlMute			B	Mute on
 *	CdlDemute		B	Mute off
 *	CdlBetfilter	B	Set SubHeader filter
 *	CdlSetmode		B	Set mode
 *	CdlGetlocL		B	Get logical position
 *	CdlGetlocP		B	Get phisycal position
 *	CdlSeekL	  	N	Logical Seek
 *	CdlSeekP	  	N	Phisical Seek
 *	CdlReadS	  	B	Read without retry
 *	CdlReset	  	B	Reset
 *	------------------------------------------------------
 *			B: Blocking, N: Non-Blocking operation
 *
		
 *	
 *	Symbol		arg				result
 *	--------------------------------------------------------------
 *	CdlNop			-				status
 *	CdlSetloc	min,sec,sector		status
 *	CdlPlay       	-				status
 *	CdlForward		-				status
 *	CdlBackword		-				status
 *	CdlReadN	  	-				status
 *	CdlStanby		-				status
 *	CdlStop			-				status
 *	CdlPause	  	-				status
 *	CdlMute			-				status
 *	CdlDemute		-				status
 *	CdlSetfilter	file,chan	status
 *	CdlSetmode		mode			status
 *	CdlGetlocL		-				min,sec,sector,mode,file, chan
 *	CdlGetlocP		-				track,index,min,sec,frame,
 *										amin,asec,aframe
 *	CdlSeekL	  	-				status
 *	CdlSeekP	  	-				status
 *	CdlReadS	  	-				status
 *	CdlReset	  	-				status
 *	--------------------------------------------------------------
 */

/*
 * CD-ROM Basic System
 */

/*
 * CD-ROM Mode
 */

#define CdlModeSpeed		0x80		/* 0: normal speed	1: double speed	*/
#define CdlModeRT			0x40		/* 0: ADPCM off		1: ADPCM on	*/
#define CdlModeSize1		0x20		/* 0: 2048 byte		1: 2340byte	*/
#define CdlModeSize0		0x10		/* 0: -			1: 2328byte	*/
#define CdlModeSF			0x08		/* 0: Channel off	1: Channel on	*/
#define CdlModeRept 		0x04	 	/* 0: Report off	1: Report on	*/
#define CdlModeAP			0x02		/* 0: AutoPause off	1: AutoPause on */
#define CdlModeDA			0x01		/* 0: CD-DA off		1: CD-DA on	*/

/*
 * CD-ROM Primitive Commands
 */

#define CdlNop			0x01	
#define CdlSetloc		0x02	
#define CdlPlay			0x03	
#define CdlForward		0x04	
#define CdlBackward		0x05
#define CdlReadN		0x06
#define CdlStandby		0x07
#define CdlStop			0x08
#define CdlPause		0x09
#define CdlReset		0x0a
#define CdlMute			0x0b
#define CdlDemute		0x0c
#define CdlSetfilter	0x0d
#define CdlSetmode		0x0e
#define CdlGetparam		0x0f
#define CdlGetlocL		0x10
#define CdlGetlocP		0x11
#define CdlReadT		0x12
#define CdlGetTN		0x13
#define CdlGetTD		0x14
#define CdlSeekL		0x15
#define CdlSeekP		0x16
#define CdlReadS		0x1B

/*
 * Interrupts
 */

#define CdlNoIntr			0x00		/* No interrupt */
#define CdlDataReady		0x01		/* Data Ready */
#define CdlComplete			0x02		/* Command Complete */
#define CdlAcknowledge		0x03		/* Acknowledge (reserved) */
#define CdlDataEnd			0x04		/* End of Data Detected */
#define CdlDiskError		0x05		/* Error Detected */

/*
 * Library Macros
 */

#define btoi(b)		((b)/16*10 + (b)%16)		/* BCD to u_char */
#define itob(i)		((i)/10*16 + (i)%10)		/* u_char to BCD */

#define CdSeekL(p)	CdControl(CdlSeekL, (u_char *)p, 0)
#define CdSeekP(p)	CdControl(CdlSeekP, (u_char *)p, 0)
/* #define CdGetToc(table)	CdGetToc2(1, table) */
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
#define CdlStbSM		0x08
#define CdlStbRead	0x04
#define CdlStbSeek	0x02
#define CdlStbBusy	0x01

/*
 *	Position
 */

#define CdlMAXTOC	100
struct CdlPos
 {
	unsigned char minute;
	unsigned char second;
	unsigned char sector;
	unsigned char track;
 };
typedef struct CdlPos CdlLOC;

/*
 *	Attenuator
 */

struct ATV
 {
	unsigned char	val0;
	unsigned char	val1;
	unsigned char	val2;
	unsigned char	val3;
 };
typedef struct ATV CdlATV;

/*
 *	File System
 */

typedef struct
 {
	CdlLOC	pos;
	unsigned long	size;
	char	name[/*12*/16];
 } CdlFILE;


/*
 *	Streaming Structures
 */

typedef struct
 {
	unsigned short	id;			/* always 0x0x0160 */
	unsigned short	type;			
	unsigned short	secCount;	
	unsigned short	nSectors;
	unsigned long	frameCount;
	unsigned long	frameSize;
	
	unsigned short	width;
	unsigned short	height;
	
	unsigned long	headm;
	unsigned long	headv;
} StSECTOR;				/* CD-ROM STR \‘¢‘Ì */

typedef struct 
 {
  char Command_Code;
  char Track;
  char Frame;
  char Sec;
  char Min;
  char Unused;
  char CtrlInfo;
 }TNOInfo;

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

/*extern void	CdInit();  !!! PUT BACK AFTER TEST !!!*/
extern int	CdSetDebug(int);
extern char	*CdComstr(unsigned char);
extern char	*CdIntstr(unsigned char);
/* extern int	CdGetToc2(int, CdlLOC *); */
extern int	CdControl(unsigned char, struct CdlPos *, unsigned char *);
extern int	CdControlB(unsigned char, unsigned char *, unsigned char *);
extern int	CdControlF(unsigned char, unsigned char *);
extern int	CdSync(int, unsigned char *);
extern int	CdGetSector(void *, int);
extern int	CdRead(int, unsigned long *, int);
extern int	CdSetMode(long);
extern int	CdRead2(long);
extern int	CdSetFilter(unsigned char, unsigned char);
extern int	CdMix(CdlATV *);
extern unsigned long	CdSyncCallback(void (*func)());
extern unsigned long	CdReadyCallback(void (*func)());
extern CdlFILE *CdSearchFile(CdlFILE *, char *);
extern CdlLOC	*CdIntToPos(int, CdlLOC *);
extern int	CdPosToIntw(CdlLOC *);

/*
 *	Prototypes for Streaming
 */

extern void	StSetRing(unsigned long *ring_addr,unsigned long ring_size);
extern void	StClearRing(void);
extern void	StUnSetRing(void);
extern unsigned long	StFreeRing();
extern void	StSetStream(unsigned long mode,unsigned long start_frame,unsigned long end_frame,
		    int (*func1)(),int (*func2)());
extern void	StSetEmulate(unsigned long *addr,unsigned long mode,unsigned long start_frame,
		     unsigned long end_frame,int (*func1)(),int (*func2)());
extern unsigned long	StFreeRing(unsigned long *base);
extern unsigned long	StGetNext(unsigned long **addr,unsigned long **header);
extern void	StSetMask(unsigned long mask,unsigned long start,unsigned long end);
extern void	StCdInterrupt(void);


extern unsigned char		BCDToBin16(unsigned char BCD);
extern unsigned char		BinToBCD16(unsigned char Bin);


/* PC Specific Stuff - Chris */

#define INIT_MP_INT     	  0x1500
#define CDREQ_MP_INT    	  0x1510

#define CD_PLAY_COMMAND       132
#define CD_PAUSE_COMMAND      133
#define CD_RESUME_COMMAND     136

#define CD_RESET_COMMAND      2

#define GET_AUDIO_DISKINFO    10
#define GET_AUDIO_TRACKINFO   11
#define GET_Q_CHANNEL_INFO	  12	
#define GET_AUDIO_STATUSINFO  15

#define READ_IOCTL_COMMAND    3
#define WRITE_IOCTL_COMMAND   12

#define ADDR_HSG              0

typedef struct
 {
  unsigned char CommandCode;
  unsigned char LoTrack;
  unsigned char HiTrack;
  unsigned long LeadOut;
 } CDDIStru;

typedef struct
 {
  unsigned char CommandCode;
  unsigned char TrackNumber;
  unsigned long StartAddress;
  unsigned char Control;
 } CDTIStru;

typedef struct
 {
  unsigned char CommandCode;
 } CDResetStru;

typedef struct
 {
  // Request Header
  unsigned char	 ReqHdrLength;
  unsigned char	 SubUnit;
  unsigned char	 CommandCode;
  unsigned short Status;
  unsigned long	 Reserved1;
  unsigned long	 Reserved2;

  // Request/Strategy Details
  unsigned char	 AddressMode;
  void     		 *BeginPointer;
  unsigned long	 Length;
  unsigned short SSNumber;
  unsigned long  VolIDPtr;
 } CDCmdStru;

typedef struct
 {
  unsigned char  CommandCode;
  unsigned short AudioStatus;
  unsigned long  StartLoc;
  unsigned long  EndLoc;
 } CDStatusStru;

typedef struct
 {
  unsigned char CommandCode;
  unsigned char ADR;
  unsigned char TNO;
  unsigned char Index;
  unsigned char TrackMin;
  unsigned char TrackSec;
  unsigned char TrackFrame;
  unsigned char TrackPad;
  unsigned char DiskMin;
  unsigned char DiskSec;
  unsigned char DiskFrame;
 } QChanStru;

typedef unsigned short WORD;
typedef unsigned long DWORD;

#define	D32RealSeg(P)	((((DWORD) (P)) >> 4) & 0xFFFF)
#define	D32RealOff(P)	(((DWORD) (P)) & 0xF)


typedef struct _dpmi_callregs
 {
  DWORD edi, esi, ebp, esp;
  DWORD ebx, edx, ecx, eax;
  WORD flags;
  WORD es, ds;
  WORD fs, gs;
  WORD ip, cs;
  WORD sp, ss;
 } DPMI_CALLREGS;


/* Function Prototypes */

void *D32DosMemAlloc (DWORD size, WORD *selector);
void CDInit();
void SendCDCommand(long Address, long Size, unsigned char CommandCmnd);
void GetDiskInfo();
void GetTrackInfo(char Track);
void GetAudioAddr(char Track);
void Reset();
unsigned long Red2HSG(unsigned long RedValue);
char CDPaused(); 
void CDResume();
void CDStop();
void CDPlay();
void CDLoopCheck();

//extern char CdPaused;
