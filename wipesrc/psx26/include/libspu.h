#ifndef _LIBSPU_H_
#define _LIBSPU_H_

/* $Id: libspu.h,v 1.31 1994/11/08 09:55:08 kaol Exp $ */

/*****************************************************************
 * -*- c -*-
 * $RCSfile: libspu.h,v $
 *
 * Copyright (C) 1994 by Sony Computer Entertainment Inc.
 *				     All Rights Reserved.
 *
 *	Sony Computer Entertainment Inc. Development Department
 *
 *****************************************************************/

/* ----------------------------------------------------------------
 *	定数
 * ---------------------------------------------------------------- */

#define SpuDiag		(-2)
#define SpuCheck	(-1)
#define SpuOff		0
#define SpuOn		1
#define SpuClear	2
#define SpuReset	3
#define SpuDontCare	4
#define SpuALL		0
#define SpuCDOnly	5
#define SpuCont		6

#define SpuOffEnvOn	2
#define SpuOnEnvOff	3

#define SpuError	(-1)

#define SpuTransferByDMA 0L
#define SpuTransferByIO 1L

#define SpuTransByDMA SpuTransferByDMA
#define SpuTransByIO SpuTransferByIO

#define SPU_TRANSFER_WAIT 1
#define SPU_TRANSFER_PEEK 0
#define SPU_TRANSFER_GLANCE SPU_TRANSFER_PEEK

/* Voice 指定 */

#ifndef __SPU_VOICE
#define __SPU_VOICE

#define SPU_0CH  (0x1L<< 0)
#define SPU_1CH  (0x1L<< 1)
#define SPU_2CH  (0x1L<< 2)
#define SPU_3CH  (0x1L<< 3)
#define SPU_4CH  (0x1L<< 4)
#define SPU_5CH  (0x1L<< 5)
#define SPU_6CH  (0x1L<< 6)
#define SPU_7CH  (0x1L<< 7)
#define SPU_8CH  (0x1L<< 8)
#define SPU_9CH  (0x1L<< 9)
#define SPU_10CH (0x1L<<10)
#define SPU_11CH (0x1L<<11)
#define SPU_12CH (0x1L<<12)
#define SPU_13CH (0x1L<<13)
#define SPU_14CH (0x1L<<14)
#define SPU_15CH (0x1L<<15)
#define SPU_16CH (0x1L<<16)
#define SPU_17CH (0x1L<<17)
#define SPU_18CH (0x1L<<18)
#define SPU_19CH (0x1L<<19)

#define SPU_20CH (0x1L<<20)
#define SPU_21CH (0x1L<<21)
#define SPU_22CH (0x1L<<22)
#define SPU_23CH (0x1L<<23)

#define SPU_ALLCH (SPU_0CH | SPU_1CH | SPU_2CH | SPU_3CH | SPU_4CH | \
		   SPU_5CH | SPU_6CH | SPU_7CH | SPU_8CH | SPU_9CH | \
		   SPU_10CH | SPU_11CH | SPU_12CH | SPU_13CH | SPU_14CH | \
		   SPU_15CH | SPU_16CH | SPU_17CH | SPU_18CH | SPU_19CH | \
		   SPU_20CH | SPU_21CH | SPU_22CH | SPU_23CH)

#define SPU_KEYCH(x) (0x1L<<(x))
#define SPU_VOICECH(x) SPU_KEYCH(x)

#endif /* __SPU_VOICE */

/* ボイス関連 */

#define	SPU_VOICE_VOLL		(0x01 <<  0) /* 音量(左)                 */
#define	SPU_VOICE_VOLR		(0x01 <<  1) /* 音量(右)                 */
#define	SPU_VOICE_VOLMODEL	(0x01 <<  2) /* 音量モード(左)           */
#define	SPU_VOICE_VOLMODER	(0x01 <<  3) /* 音量モード(右)           */
#define	SPU_VOICE_PITCH		(0x01 <<  4) /* 音程 (ピッチ指定)        */
#define	SPU_VOICE_NOTE		(0x01 <<  5) /* 音程 (ノート指定)        */
#define	SPU_VOICE_SAMPLE_NOTE	(0x01 <<  6) /* 波形データサンプルノート */
#define	SPU_VOICE_WDSA		(0x01 <<  7) /* 波形データ先頭アドレス   */
#define	SPU_VOICE_ADSR_AMODE	(0x01 <<  8) /* ADSR Attack rate モード  */
#define	SPU_VOICE_ADSR_SMODE	(0x01 <<  9) /* ADSR Sustain rate モード */
#define	SPU_VOICE_ADSR_RMODE	(0x01 << 10) /* ADSR Release rate モード */
#define	SPU_VOICE_ADSR_AR	(0x01 << 11) /* ADSR Attack rate         */
#define	SPU_VOICE_ADSR_DR	(0x01 << 12) /* ADSR Decay rate          */
#define	SPU_VOICE_ADSR_SR	(0x01 << 13) /* ADSR Sustain rate        */
#define	SPU_VOICE_ADSR_RR	(0x01 << 14) /* ADSR Release rate        */
#define	SPU_VOICE_ADSR_SL	(0x01 << 15) /* ADSR Sustain level       */
#define	SPU_VOICE_LSAX		(0x01 << 16) /* ループ開始アドレス       */

#define	SPU_VOICE_DIRECT	0
#define	SPU_VOICE_LINEARIncN	1
#define	SPU_VOICE_LINEARIncR	2
#define	SPU_VOICE_LINEARDecN	3
#define	SPU_VOICE_LINEARDecR	4
#define	SPU_VOICE_EXPIncN	5
#define	SPU_VOICE_EXPIncR	6
#define	SPU_VOICE_EXPDec	7
#define	SPU_VOICE_EXPDecN	SPU_VOICE_EXPDec
#define	SPU_VOICE_EXPDecR	SPU_VOICE_EXPDec

#define	SPU_DECODE_FIRSTHALF	0
#define	SPU_DECODE_SECONDHALF	1

#define	SPU_COMMON_MVOLL		(0x01 <<  0) /* マスタ音量(左)                     */
#define	SPU_COMMON_MVOLR		(0x01 <<  1) /* マスタ音量(右)                     */
#define	SPU_COMMON_MVOLMODEL		(0x01 <<  2) /* マスタ音量モード(左)               */
#define	SPU_COMMON_MVOLMODER		(0x01 <<  3) /* マスタ音量モード(右)               */
#define	SPU_COMMON_RVOLL		(0x01 <<  4) /* リバーブ音量(左)                   */
#define	SPU_COMMON_RVOLR		(0x01 <<  5) /* リバーブ音量(右)                   */
#define	SPU_COMMON_CDVOLL		(0x01 <<  6) /* CD 入力 音量 (左)                  */
#define	SPU_COMMON_CDVOLR		(0x01 <<  7) /* CD 入力 音量 (右)                  */
#define	SPU_COMMON_CDREV		(0x01 <<  8) /* CD 入力 リバーブオン/オフ          */
#define	SPU_COMMON_CDMIX		(0x01 <<  9) /* CD 入力 オン/オフ                  */
#define	SPU_COMMON_EXTVOLL		(0x01 << 10) /* 外部デジタル入力 音量 (左)         */
#define	SPU_COMMON_EXTVOLR		(0x01 << 11) /* 外部デジタル入力 音量 (右)         */
#define	SPU_COMMON_EXTREV		(0x01 << 12) /* 外部デジタル入力 リバーブオン/オフ */
#define	SPU_COMMON_EXTMIX		(0x01 << 13) /* 外部デジタル入力 オン/オフ         */

/* リバーブ関連 */

#define	SPU_REV_MODE		(0x01 <<  0) /* モード設定                     */
#define	SPU_REV_DEPTHL		(0x01 <<  1) /* リバーブの深さ(左)             */
#define	SPU_REV_DEPTHR		(0x01 <<  2) /* リバーブの深さ(右)             */
#define	SPU_REV_DELAYTIME	(0x01 <<  3) /* Delay Time (ECHO, DELAY のみ)  */
#define	SPU_REV_FEEDBACK	(0x01 <<  4) /* Feedback   (ECHO のみ)         */

#define SPU_REV_MODE_CHECK	(-1)
#define SPU_REV_MODE_OFF	0
#define SPU_REV_MODE_ROOM	1
#define SPU_REV_MODE_STUDIO_A	2
#define SPU_REV_MODE_STUDIO_B	3
#define SPU_REV_MODE_STUDIO_C	4
#define SPU_REV_MODE_HALL	5
#define SPU_REV_MODE_SPACE	6
#define SPU_REV_MODE_ECHO	7
#define SPU_REV_MODE_DELAY	8
#define SPU_REV_MODE_PIPE	9
#define SPU_REV_MODE_MAX	10

#define SPU_REV_MODE_CLEAR_WA	0x100

/* ----------------------------------------------------------------
 *	構造体
 * ---------------------------------------------------------------- */

typedef struct {
    short left;	       /* Lch */
    short right;       /* Rch */
} SpuVolume;

typedef struct {
    unsigned short left;	/* Lch */
    unsigned short right;       /* Rch */
} SpuVolume16;

typedef struct {
    unsigned long	voice;		/* 設定ボイス:
					   SpuSetVoiceAttr: 各ボイスは bit 列
					   SpuGetVoiceAttr: 0 ～ 23 の値
					   */
    unsigned long	mask;		/* 設定属性ビット (Get では無効)	*/
    SpuVolume		volume;		/* 音量					*/
    SpuVolume		volmode;	/* 音量モード				*/
    SpuVolume		volumex;	/* 現在の音量 (Set では無効)		*/
    unsigned short	pitch;		/* 音程 (ピッチ指定)			*/
    unsigned short	note;		/* 音程 (ノート指定)			*/
    unsigned short	sample_note;	/* 音程 (ノート指定)			*/
#ifdef OCT_19TH
    unsigned short	envx;		/* 現在のエンベロープ値 (Set では無効)  */
#else
    short		envx;		/* 現在のエンベロープ値 (Set では無効)  */
#endif /* OCT_19TH */
    unsigned long	addr;		/* 波形データ先頭アドレス		*/
    unsigned long	loop_addr;	/* ループ開始アドレス			*/
    long		a_mode;		/* Attack rate モード			*/
    long		s_mode;		/* Sustain rate モード			*/
    long		r_mode;		/* Release rate モード			*/
    unsigned short	ar;		/* Attack rate				*/
    unsigned short	dr;		/* Decay rate				*/
    unsigned short	sr;		/* Sustain rate				*/
    unsigned short	rr;		/* Release rate				*/
    unsigned short	sl;		/* Sustain level			*/

} SpuVoiceAttr;

typedef struct {
    unsigned long	mask;	  /* 設定マスク		  */
    
    long		mode;	  /* リバーブモード       */
    SpuVolume		depth;	  /* リバーブの深さ	  */
    long                delay;	  /* Delay Time (ECHO, DELAY のみ)  */
    long                feedback; /* Feedback   (ECHO のみ)         */
} SpuReverbAttr;

#define SPU_DECODEDATA_SIZE 0x200
typedef struct {
    short cd_left  [SPU_DECODEDATA_SIZE];
    short cd_right [SPU_DECODEDATA_SIZE];
    short voice1   [SPU_DECODEDATA_SIZE];
    short voice3   [SPU_DECODEDATA_SIZE];
} SpuDecodeData;

typedef struct {
    SpuVolume	volume;		  /* 音量		  */
    long	reverb;		  /* リバーブオン/オフ	  */
    long	mix;		  /* ミキシングオン/オフ  */
} SpuExtAttr;

typedef struct {
    unsigned long	mask;	  /* 設定マスク		  */
    
    SpuVolume		mvol;	  /* マスタ音量		  */
    SpuVolume		mvolmode; /* マスタ音量モード	  */
    SpuVolume		mvolx;	  /* 現在のマスタ音量	  */
    SpuExtAttr		cd;	  /* CD 入力属性	  */
    SpuExtAttr		ext;	  /* 外部デジタル入力属性 */
} SpuCommonAttr;

typedef void (*SpuIRQCallbackProc)(void);

/* ----------------------------------------------------------------
 *	SPU Malloc 用
 * ---------------------------------------------------------------- */

#define SPU_MALLOC_RECSIZ 8

/* ----------------------------------------------------------------
 *	プロトタイプ宣言
 * ---------------------------------------------------------------- */

extern void SpuInit (void);
extern void SpuStart (void);
extern void SpuQuit (void);
extern long SpuSetMute (long on_off);
extern long SpuGetMute (void);

extern long SpuSetNoiseClock (long n_clock);
extern long SpuGetNoiseClock (void);
extern unsigned long SpuSetNoiseVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetNoiseVoice (void);

extern long SpuSetReverb (long on_off);
extern long SpuGetReverb (void);
extern long SpuSetReverbModeParam (SpuReverbAttr *attr);
extern void SpuGetReverbModeParam (SpuReverbAttr *attr);
extern long SpuReserveReverbWorkArea (long on_off);
extern long SpuIsReverbWorkAreaReserved (long on_off);
extern unsigned long SpuSetReverbOffsetAddr (unsigned long addr);
extern unsigned long SpuGetReverbOffsetAddr (void);
extern unsigned long SpuSetReverbVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetReverbVoice (void);
extern long SpuClearReverbWorkArea (long mode);

extern unsigned long SpuWrite (unsigned char *addr, unsigned long size);
extern unsigned long SpuWrite0 (unsigned long size);
extern unsigned long SpuRead (unsigned char *addr, unsigned long size);
extern long SpuSetTransferMode (long mode);
#define SpuSetTransMode(mode) SpuSetTransferMode((mode))
extern long SpuGetTransferMode (void);
#define SpuGetTransMode() SpuGetTransferMode()
extern unsigned long SpuSetTransferStartAddr (unsigned long addr);
#define SpuSetTransStartAddr(addr) SpuSetTransferStartAddr((addr))
extern unsigned long SpuGetTransferStartAddr (void);
#define SpuGetTransStartAddr() SpuGetTransferStartAddr()
extern unsigned long SpuWritePartly (unsigned char *addr, unsigned long size);

extern long SpuIsTransferCompleted (long flag);

extern long SpuReadDecodeData (SpuDecodeData *d_data, long flag);

extern long SpuSetIRQ (long on_off);
extern long SpuGetIRQ (void);
extern unsigned long SpuSetIRQAddr (unsigned long);
extern unsigned long SpuGetIRQAddr (void);
extern void SpuSetIRQCallback (SpuIRQCallbackProc);

extern void SpuSetVoiceAttr (SpuVoiceAttr *arg);
extern void SpuGetVoiceAttr (SpuVoiceAttr *arg);
extern void SpuSetKey (long on_off, unsigned long voice_bit);
extern void SpuSetKeyOnWithAttr (SpuVoiceAttr *attr);
extern long SpuGetKeyStatus (unsigned long voice_bit);
extern void SpuGetAllKeysStatus (char *status);

extern unsigned long SpuSetPitchLFOVoice (long on_off, unsigned long voice_bit);
extern unsigned long SpuGetPitchLFOVoice (void);

extern long SpuSetCommonAttr (SpuCommonAttr *attr);
extern void SpuGetCommonAttr (SpuCommonAttr *attr);

extern long SpuInitMalloc (long num, char *top);
extern long SpuMalloc (long size);
extern long SpuMallocWithStartAddr (unsigned long addr, long size);
extern void SpuFree (unsigned long addr);

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
#endif /* _LIBSPU_H_ */
/* DON'T ADD STUFF AFTER THIS */
