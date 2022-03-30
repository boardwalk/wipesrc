/*
 * File:kernel.h 	Rev. 3
#include <r3000.h>
*/

/* don't change these macros and structures which is refereced in kernel code */
#define	LANGUAGE_C
#include "wipesrc/psx26/include/asm.h"  // PCwipeout


#define DescMask 	0xff000000
#define DescTH 		DescMask
#define DescHW   	0xf0000000
#define DescEV   	0xf1000000
#define DescRC   	0xf2000000
#define DescUEV   	0xf3000000	/* User event */
#define DescSW   	0xf4000000	/* BIOS */

#define HwVBLANK	(DescHW|0x01)	/* VBLANK */
#define HwGPU		(DescHW|0x02)	/* GPU */
#define HwCdRom		(DescHW|0x03)	/* CDROM Decorder */
#define HwDMAC		(DescHW|0x04)	/* DMA controller */
#define HwRTC0		(DescHW|0x05)	/* RTC0 */
#define HwRTC1		(DescHW|0x06)	/* RTC1 */
#define HwRTC2		(DescHW|0x07)	/* RTC2 */
#define HwCNTL		(DescHW|0x08)	/* Controller */
#define HwSPU		(DescHW|0x09)	/* SPU */
#define HwPIO		(DescHW|0x0a)	/* PIO */
#define HwSIO		(DescHW|0x0b)	/* SIO */

#define HwCPU		(DescHW|0x10)	/* Exception */
#define HwCARD		(DescHW|0x11)	/* memory card */
#define HwCARD_0	(DescHW|0x12)	/* memory card */
#define HwCARD_1	(DescHW|0x13)	/* memory card */
#define SwCARD		(DescSW|0x01)	/* memory card */

#define RCntCNT0     	(DescRC|0x00)  	/* �\���s�N�Z�� */
#define RCntCNT1  	(DescRC|0x01)  	/* �������� */
#define RCntCNT2  	(DescRC|0x02)  	/* �V�X�e���N���b�N�W���� */
#define RCntCNT3  	(DescRC|0x03)  	/* �������� �^�[�Q�b�g�l�͂P�ɌŒ� */

#define RCntMdINTR	0x1000
#define RCntMdNOINTR	0x2000
#define RCntMdSC	0x0001
#define RCntMdSP	0x0000
#define RCntMdFR	0x0000
#define RCntMdGATE	0x0010

#define EvSpCZ		0x0001		/* counter becomes zero */
#define EvSpINT		0x0002		/* interrupted */
#define EvSpIOE		0x0004		/* end of i/o */
#define EvSpCLOSE	0x0008		/* file was closed */
#define EvSpACK		0x0010		/* command acknowledged */
#define EvSpCOMP	0x0020		/* command completed */
#define EvSpDR		0x0040		/* data ready */
#define EvSpDE		0x0080		/* data end */
#define EvSpTIMOUT      0x0100          /* time out */
#define EvSpUNKNOWN     0x0200          /* unknown command */
#define EvSpIOER	0x0400		/* end of read buffer */
#define EvSpIOEW	0x0800		/* end of write buffer */
#define EvSpTRAP       	0x1000          /* general interrupt */
#define EvSpNEW		0x2000		/* new device */
#define EvSpSYSCALL	0x4000		/* system call instruction */
#define EvSpERROR	0x8000		/* error happned */
#define EvSpPERROR	0x8001		/* previous write error happned */

#define EvMdINTR	0x1000
#define EvMdNOINTR	0x2000

#define EvStUNUSED    	0x0000
#define EvStWAIT    	0x1000
#define EvStACTIVE    	0x2000
#define EvStALREADY   	0x4000

#define TcbMdRT		0x1000		/* reserved by system */
#define TcbMdPRI	0x2000		/* reserved by system */

#define TcbStUNUSED	0x1000
#define	TcbStACTIVE	0x4000

struct ToT {
	uint32_t *head;
	int32_t size;
};

struct TCBH {
	struct TCB *entry;	/* NULL */
};

struct TCB {
	int32_t status;
	int32_t mode;
	uint32_t reg[NREGS];	/* never change the offset of this */
	int32_t system[6];			/* reserved by system */
};

struct EvCB {
	uint32_t desc;
	int32_t status;
	int32_t spec;
	int32_t mode;
	int32_t (*FHandler)();
	int32_t system[2];			/* reserved by system */
};


struct EXEC {
        uint32_t pc0;
        uint32_t gp0;
        uint32_t t_addr;
        uint32_t t_size;
        uint32_t d_addr;
        uint32_t d_size;
        uint32_t b_addr;
        uint32_t b_size;
	uint32_t s_addr;
	uint32_t s_size;
	uint32_t aysp,ayfp,aygp,ayret,base;
};


struct XF_HDR {
	char key[8];
	uint32_t text;
	uint32_t data;
	struct EXEC exec;
	char title[60];		/* "PlayStation(tm) Executable A1" */
};


struct DIRENTRY {
	char name[20];
	int32_t attr;
	int32_t size;
	struct DIRENTRY *next;
	int32_t head;
	char system[4];
};


extern struct ToT SysToT[32];

extern int32_t SysClearRCnt[];

#ifndef NULL
#define NULL (0)
#endif
