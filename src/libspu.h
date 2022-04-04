#ifndef LIBSPU_H
#define LIBSPU_H

#include <stdint.h>
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
 *	�萔
 * ---------------------------------------------------------------- */

#define SpuDiag (-2)
#define SpuCheck (-1)
#define SpuOff 0
#define SpuOn 1
#define SpuClear 2
#define SpuReset 3
#define SpuDontCare 4
#define SpuALL 0
#define SpuCDOnly 5
#define SpuCont 6

#define SpuOffEnvOn 2
#define SpuOnEnvOff 3

#define SpuError (-1)

#define SpuTransferByDMA 0L
#define SpuTransferByIO 1L

#define SpuTransByDMA SpuTransferByDMA
#define SpuTransByIO SpuTransferByIO

#define SPU_TRANSFER_WAIT 1
#define SPU_TRANSFER_PEEK 0
#define SPU_TRANSFER_GLANCE SPU_TRANSFER_PEEK

/* Voice �w�� */

#ifndef __SPU_VOICE
#define __SPU_VOICE

#define SPU_0CH (0x1L << 0)
#define SPU_1CH (0x1L << 1)
#define SPU_2CH (0x1L << 2)
#define SPU_3CH (0x1L << 3)
#define SPU_4CH (0x1L << 4)
#define SPU_5CH (0x1L << 5)
#define SPU_6CH (0x1L << 6)
#define SPU_7CH (0x1L << 7)
#define SPU_8CH (0x1L << 8)
#define SPU_9CH (0x1L << 9)
#define SPU_10CH (0x1L << 10)
#define SPU_11CH (0x1L << 11)
#define SPU_12CH (0x1L << 12)
#define SPU_13CH (0x1L << 13)
#define SPU_14CH (0x1L << 14)
#define SPU_15CH (0x1L << 15)
#define SPU_16CH (0x1L << 16)
#define SPU_17CH (0x1L << 17)
#define SPU_18CH (0x1L << 18)
#define SPU_19CH (0x1L << 19)

#define SPU_20CH (0x1L << 20)
#define SPU_21CH (0x1L << 21)
#define SPU_22CH (0x1L << 22)
#define SPU_23CH (0x1L << 23)

#define SPU_ALLCH (SPU_0CH | SPU_1CH | SPU_2CH | SPU_3CH | SPU_4CH |      \
                   SPU_5CH | SPU_6CH | SPU_7CH | SPU_8CH | SPU_9CH |      \
                   SPU_10CH | SPU_11CH | SPU_12CH | SPU_13CH | SPU_14CH | \
                   SPU_15CH | SPU_16CH | SPU_17CH | SPU_18CH | SPU_19CH | \
                   SPU_20CH | SPU_21CH | SPU_22CH | SPU_23CH)

#define SPU_KEYCH(x) (0x1L << (x))
#define SPU_VOICECH(x) SPU_KEYCH(x)

#endif /* __SPU_VOICE */

/* �{�C�X�֘A */

#define SPU_VOICE_VOLL (0x01 << 0) /* ����(��)                 */
#define SPU_VOICE_VOLR (0x01 << 1) /* ����(�E)                 */
#define SPU_VOICE_VOLMODEL (0x01 << 2) /* ���ʃ��[�h(��)           */
#define SPU_VOICE_VOLMODER (0x01 << 3) /* ���ʃ��[�h(�E)           */
#define SPU_VOICE_PITCH (0x01 << 4) /* ���� (�s�b�`�w��)        */
#define SPU_VOICE_NOTE (0x01 << 5) /* ���� (�m�[�g�w��)        */
#define SPU_VOICE_SAMPLE_NOTE (0x01 << 6) /* �g�`�f�[�^�T���v���m�[�g */
#define SPU_VOICE_WDSA (0x01 << 7) /* �g�`�f�[�^�擪�A�h���X   */
#define SPU_VOICE_ADSR_AMODE (0x01 << 8) /* ADSR Attack rate ���[�h  */
#define SPU_VOICE_ADSR_SMODE (0x01 << 9) /* ADSR Sustain rate ���[�h */
#define SPU_VOICE_ADSR_RMODE (0x01 << 10) /* ADSR Release rate ���[�h */
#define SPU_VOICE_ADSR_AR (0x01 << 11) /* ADSR Attack rate         */
#define SPU_VOICE_ADSR_DR (0x01 << 12) /* ADSR Decay rate          */
#define SPU_VOICE_ADSR_SR (0x01 << 13) /* ADSR Sustain rate        */
#define SPU_VOICE_ADSR_RR (0x01 << 14) /* ADSR Release rate        */
#define SPU_VOICE_ADSR_SL (0x01 << 15) /* ADSR Sustain level       */
#define SPU_VOICE_LSAX (0x01 << 16) /* ���[�v�J�n�A�h���X       */

#define SPU_VOICE_DIRECT 0
#define SPU_VOICE_LINEARIncN 1
#define SPU_VOICE_LINEARIncR 2
#define SPU_VOICE_LINEARDecN 3
#define SPU_VOICE_LINEARDecR 4
#define SPU_VOICE_EXPIncN 5
#define SPU_VOICE_EXPIncR 6
#define SPU_VOICE_EXPDec 7
#define SPU_VOICE_EXPDecN SPU_VOICE_EXPDec
#define SPU_VOICE_EXPDecR SPU_VOICE_EXPDec

#define SPU_DECODE_FIRSTHALF 0
#define SPU_DECODE_SECONDHALF 1

#define SPU_COMMON_MVOLL (0x01 << 0) /* �}�X�^����(��)                     */
#define SPU_COMMON_MVOLR (0x01 << 1) /* �}�X�^����(�E)                     */
#define SPU_COMMON_MVOLMODEL (0x01 << 2) /* �}�X�^���ʃ��[�h(��)               */
#define SPU_COMMON_MVOLMODER (0x01 << 3) /* �}�X�^���ʃ��[�h(�E)               */
#define SPU_COMMON_RVOLL (0x01 << 4) /* ���o�[�u����(��)                   */
#define SPU_COMMON_RVOLR (0x01 << 5) /* ���o�[�u����(�E)                   */
#define SPU_COMMON_CDVOLL (0x01 << 6) /* CD ���� ���� (��)                  */
#define SPU_COMMON_CDVOLR (0x01 << 7) /* CD ���� ���� (�E)                  */
#define SPU_COMMON_CDREV (0x01 << 8) /* CD ���� ���o�[�u�I��/�I�t          */
#define SPU_COMMON_CDMIX (0x01 << 9) /* CD ���� �I��/�I�t                  */
#define SPU_COMMON_EXTVOLL (0x01 << 10) /* �O���f�W�^������ ���� (��)         */
#define SPU_COMMON_EXTVOLR (0x01 << 11) /* �O���f�W�^������ ���� (�E)         */
#define SPU_COMMON_EXTREV (0x01 << 12) /* �O���f�W�^������ ���o�[�u�I��/�I�t */
#define SPU_COMMON_EXTMIX (0x01 << 13) /* �O���f�W�^������ �I��/�I�t         */

/* ���o�[�u�֘A */

#define SPU_REV_MODE (0x01 << 0) /* ���[�h�ݒ�                     */
#define SPU_REV_DEPTHL (0x01 << 1) /* ���o�[�u�̐[��(��)             */
#define SPU_REV_DEPTHR (0x01 << 2) /* ���o�[�u�̐[��(�E)             */
#define SPU_REV_DELAYTIME (0x01 << 3) /* Delay Time (ECHO, DELAY �̂�)  */
#define SPU_REV_FEEDBACK (0x01 << 4) /* Feedback   (ECHO �̂�)         */

#define SPU_REV_MODE_CHECK (-1)
#define SPU_REV_MODE_OFF 0
#define SPU_REV_MODE_ROOM 1
#define SPU_REV_MODE_STUDIO_A 2
#define SPU_REV_MODE_STUDIO_B 3
#define SPU_REV_MODE_STUDIO_C 4
#define SPU_REV_MODE_HALL 5
#define SPU_REV_MODE_SPACE 6
#define SPU_REV_MODE_ECHO 7
#define SPU_REV_MODE_DELAY 8
#define SPU_REV_MODE_PIPE 9
#define SPU_REV_MODE_MAX 10

#define SPU_REV_MODE_CLEAR_WA 0x100

/* ----------------------------------------------------------------
 *	�\����
 * ---------------------------------------------------------------- */

typedef struct {
  int16_t left; /* Lch */
  int16_t right; /* Rch */
} SpuVolume;

typedef struct {
  uint16_t left; /* Lch */
  uint16_t right; /* Rch */
} SpuVolume16;

typedef struct {
  uint32_t voice; /* �ݒ�{�C�X:
					   SpuSetVoiceAttr: �e�{�C�X�� bit ��
					   SpuGetVoiceAttr: 0 �` 23 �̒l
					   */
  uint32_t mask; /* �ݒ葮���r�b�g (Get �ł͖���)	*/
  SpuVolume volume; /* ����					*/
  SpuVolume volmode; /* ���ʃ��[�h				*/
  SpuVolume volumex; /* ���݂̉��� (Set �ł͖���)		*/
  uint16_t pitch; /* ���� (�s�b�`�w��)			*/
  uint16_t note; /* ���� (�m�[�g�w��)			*/
  uint16_t sample_note; /* ���� (�m�[�g�w��)			*/
#ifdef OCT_19TH
  uint16_t envx; /* ���݂̃G���x���[�v�l (Set �ł͖���)  */
#else
  int16_t envx; /* ���݂̃G���x���[�v�l (Set �ł͖���)  */
#endif /* OCT_19TH */
  uint32_t addr; /* �g�`�f�[�^�擪�A�h���X		*/
  uint32_t loop_addr; /* ���[�v�J�n�A�h���X			*/
  int32_t a_mode; /* Attack rate ���[�h			*/
  int32_t s_mode; /* Sustain rate ���[�h			*/
  int32_t r_mode; /* Release rate ���[�h			*/
  uint16_t ar; /* Attack rate				*/
  uint16_t dr; /* Decay rate				*/
  uint16_t sr; /* Sustain rate				*/
  uint16_t rr; /* Release rate				*/
  uint16_t sl; /* Sustain level			*/

} SpuVoiceAttr;

typedef struct {
  uint32_t mask; /* �ݒ�}�X�N		  */

  int32_t mode; /* ���o�[�u���[�h       */
  SpuVolume depth; /* ���o�[�u�̐[��	  */
  int32_t delay; /* Delay Time (ECHO, DELAY �̂�)  */
  int32_t feedback; /* Feedback   (ECHO �̂�)         */
} SpuReverbAttr;

#define SPU_DECODEDATA_SIZE 0x200
typedef struct {
  int16_t cd_left[SPU_DECODEDATA_SIZE];
  int16_t cd_right[SPU_DECODEDATA_SIZE];
  int16_t voice1[SPU_DECODEDATA_SIZE];
  int16_t voice3[SPU_DECODEDATA_SIZE];
} SpuDecodeData;

typedef struct {
  SpuVolume volume; /* ����		  */
  int32_t reverb; /* ���o�[�u�I��/�I�t	  */
  int32_t mix; /* �~�L�V���O�I��/�I�t  */
} SpuExtAttr;

typedef struct {
  uint32_t mask; /* �ݒ�}�X�N		  */

  SpuVolume mvol; /* �}�X�^����		  */
  SpuVolume mvolmode; /* �}�X�^���ʃ��[�h	  */
  SpuVolume mvolx; /* ���݂̃}�X�^����	  */
  SpuExtAttr cd; /* CD ���͑���	  */
  SpuExtAttr ext; /* �O���f�W�^�����͑��� */
} SpuCommonAttr;

typedef void (*SpuIRQCallbackProc)();

/* ----------------------------------------------------------------
 *	SPU Malloc �p
 * ---------------------------------------------------------------- */

#define SPU_MALLOC_RECSIZ 8

/* ----------------------------------------------------------------
 *	�v���g�^�C�v�錾
 * ---------------------------------------------------------------- */

extern void SpuInit();
extern void SpuStart();
extern void SpuQuit();
extern int32_t SpuSetMute(int32_t on_off);
extern int32_t SpuGetMute();

extern int32_t SpuSetNoiseClock(int32_t n_clock);
extern int32_t SpuGetNoiseClock();
extern uint32_t SpuSetNoiseVoice(int32_t on_off, uint32_t voice_bit);
extern uint32_t SpuGetNoiseVoice();

extern int32_t SpuSetReverb(int32_t on_off);
extern int32_t SpuGetReverb();
extern int32_t SpuSetReverbModeParam(SpuReverbAttr* attr);
extern void SpuGetReverbModeParam(SpuReverbAttr* attr);
extern int32_t SpuReserveReverbWorkArea(int32_t on_off);
extern int32_t SpuIsReverbWorkAreaReserved(int32_t on_off);
extern uint32_t SpuSetReverbOffsetAddr(uint32_t addr);
extern uint32_t SpuGetReverbOffsetAddr();
extern uint32_t SpuSetReverbVoice(int32_t on_off, uint32_t voice_bit);
extern uint32_t SpuGetReverbVoice();
extern int32_t SpuClearReverbWorkArea(int32_t mode);

extern uint32_t SpuWrite(uint8_t* addr, uint32_t size);
extern uint32_t SpuWrite0(uint32_t size);
extern uint32_t SpuRead(uint8_t* addr, uint32_t size);
extern int32_t SpuSetTransferMode(int32_t mode);
#define SpuSetTransMode(mode) SpuSetTransferMode((mode))
extern int32_t SpuGetTransferMode();
#define SpuGetTransMode() SpuGetTransferMode()
extern uint32_t SpuSetTransferStartAddr(uint32_t addr);
#define SpuSetTransStartAddr(addr) SpuSetTransferStartAddr((addr))
extern uint32_t SpuGetTransferStartAddr();
#define SpuGetTransStartAddr() SpuGetTransferStartAddr()
extern uint32_t SpuWritePartly(uint8_t* addr, uint32_t size);

extern int32_t SpuIsTransferCompleted(int32_t flag);

extern int32_t SpuReadDecodeData(SpuDecodeData* d_data, int32_t flag);

extern int32_t SpuSetIRQ(int32_t on_off);
extern int32_t SpuGetIRQ();
extern uint32_t SpuSetIRQAddr(uint32_t);
extern uint32_t SpuGetIRQAddr();
extern void SpuSetIRQCallback(SpuIRQCallbackProc);

extern void SpuSetVoiceAttr(SpuVoiceAttr* arg);
extern void SpuGetVoiceAttr(SpuVoiceAttr* arg);
extern void SpuSetKey(int32_t on_off, uint32_t voice_bit);
extern void SpuSetKeyOnWithAttr(SpuVoiceAttr* attr);
extern int32_t SpuGetKeyStatus(uint32_t voice_bit);
extern void SpuGetAllKeysStatus(char* status);

extern uint32_t SpuSetPitchLFOVoice(int32_t on_off, uint32_t voice_bit);
extern uint32_t SpuGetPitchLFOVoice();

extern int32_t SpuSetCommonAttr(SpuCommonAttr* attr);
extern void SpuGetCommonAttr(SpuCommonAttr* attr);

extern int32_t SpuInitMalloc(int32_t num, char* top);
extern int32_t SpuMalloc(int32_t size);
extern int32_t SpuMallocWithStartAddr(uint32_t addr, int32_t size);
extern void SpuFree(uint32_t addr);

/* ----------------------------------------------------------------
 *	End on File
 * ---------------------------------------------------------------- */
#endif /* _LIBSPU_H_ */
/* DON'T ADD STUFF AFTER THIS */
