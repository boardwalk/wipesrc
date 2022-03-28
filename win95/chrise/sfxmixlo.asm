;==============================================================
;
;   $Log:   P:/ridger/ridger/SFXMIXLO.ASv  $
;  
;     Rev 1.0   13 Jul 1995 14:41:08   DaveS
;  Put Back after we stopped being naughty children
;
;   Rev 1.5   08 Feb 1995 14:06:26   AndrewC
;Added full stereo mixing for 16 and 8 bit cards
;
;   Rev 1.4   18 Nov 1994 15:03:06   AndrewC
; Added routines for 8bit compatibility
;
;
;==============================================================
				        
	include flat32.i	      

	public ClearMixBuf_
	public ClearDmaBuf_

	public MonoCopyNormal_
	public MonoCopyNormalHalf_
	public MonoCopyPitch_
	public MonoCopyPitchHalf_
	public MonoMixNormal_
	public MonoMixNormalHalf_
	public MonoMixPitch_
	public MonoMixPitchHalf_

	public MonoZeroPad_

	public StereoCopyNormal_
	public StereoCopyNormalHalf_
	public StereoCopyPitch_
	public StereoCopyPitchHalf_
	public StereoMixNormal_
	public StereoMixNormalHalf_
	public StereoMixPitch_
	public StereoMixPitchHalf_

	public StereoCopyNormalSurround_
	public StereoCopyNormalSurroundHalf_
	public StereoCopyPitchSurround_
	public StereoCopyPitchSurroundHalf_
	public StereoMixNormalSurround_
	public StereoMixNormalSurroundHalf_
	public StereoMixPitchSurround_
	public StereoMixPitchSurroundHalf_

	public StereoZeroPad_

	public Mix2Dma16Mono_
	public Mix2Dma16Stereo_
	public Mix2Dma8Mono_
	public Mix2Dma8Stereo_

;
; Imported Global variables (from sfx.c)	
;
	;extern void *ppvBufferG[2];
	extern _ppvBufferG	:dword		; pointer to DMA buff pointers

	;extern int32 *pdwMixBufG;
	extern _pdwMixBufG   	:dword		; pointer to Mix Buffer

	;extern uint32 dwMixBufFramesG;
	extern _dwMixBufFramesG	:dword		; length of Mix Buffer (frames)

	;extern uint32 dwDmaBufSizeG;
	extern _dwDmaBufSizeG	:dword 		; size of Dma Buffer (bytes)

	;extern uint32 dwDmaBlockSamplesG;
      	extern _dwDmaBlockSamplesG :dword      	; size of Dma Block (samples)

	;extern uint32 dwDmaBlockSizeG;
	extern _dwDmaBlockSizeG	:dword		; size of Dma Block (bytes)

	;extern volatile uint16 wBufferFlagG;
	extern _wBufferFlagG	:word		; DMA buffer selector

	;extern uint16 wMaxActiveG;
	extern _wMaxActiveG	:word		; Maximum number of effects to mix

	;extern int16 *pwDeCmprsG;
	extern _pwDeCmprsG	:dword		; pointer to decompression table

	;extern uint16 wMaxBitsG;
	extern _wMaxBitsG	:word		; sample resolution

	
	extern _dwMixBufSamplesG :dword
	extern _wOffsetG	:word
	extern _pdwDstPtrG	:dword
	extern _pbSrcPtrG	:dword
	extern _dwLoopCountG	:dword
	extern _wIncIndxG	:word
	extern _wIncrementG	:word
	extern _bLeftOffG	:byte
	extern _bRightOffG	:byte

	START32DATA		  

	;-- local globals --
	dwThisSampleG  		dd	?
	dwNextSampleG		dd 	? 
	dwThisLeftSampleG	dd	?
	dwThisRightSampleG	dd	?
	dwNextLeftSampleG	dd	?
	dwNextRightSampleG	dd	?
	 
	END32DATA
					        
	
	START32CODE

PUSHALL	macro
	push eax
	push ebx
	push ecx
	push edx
	push esi
	push edi
	push ebp
	endm

POPALL	macro
	pop  ebp
	pop  edi
	pop  esi
	pop  edx
	pop  ecx
	pop  ebx
	pop  eax
	endm

;
; void ClearMixBuf(void)
;
ClearMixBuf_ proc near
	
	push	edi
	push	eax
	push	ecx

	;-- setup registers --
	mov	edi,_pdwMixBufG		; pointer to mix buffer
	mov	ecx,_dwMixBufSamplesG	; number of samples in buffer
	sar	ecx,2			; divide by 4
	xor	eax,eax			; clear eax

ClearMixBufLoop:
	mov	DWORD PTR[edi],eax;	; write zero
	add	edi,4			; increment pointer
	mov	DWORD PTR[edi],eax;	
	add	edi,4
	mov	DWORD PTR[edi],eax;	
	add	edi,4
	mov	DWORD PTR[edi],eax;	
	add	edi,4

	dec 	ecx			; decrement counter
	jnz	ClearMixBufLoop		; jump if not zero

	pop	ecx
	pop	eax
	pop	edi

	ret
ClearMixBuf_ endp


;
; void ClearDmaBuf(void)
;
ClearDmaBuf_ proc near
	;extern void *ppvBufferG[2];
	;extern uint32 dwDmaBlockSizeG;
	;extern uint16 wMaxBitsG;
	;extern volatile uint16 wBufferFlagG;

	PUSHALL

	mov	edi,_ppvBufferG		; get pointer to DMA buffer
	mov	ecx,_dwDmaBlockSizeG	; get number of bytes in DMA buffer
	mov	edx,ecx			; copy to edx
	shr	ecx,2			; divide by four

	;-- use 0 or 128 as zero --
	xor	eax,eax			; clear eax
	cmp	_wMaxBitsG,8		; check sample resolution
	jne	@@sixteen		
	mov	eax,80808080h		; 4 bytes of 128 for 8 bit unsigned

@@sixteen:
	;-- test wBufferFlag --
	mov	bx,_wBufferFlagG       	; get buffer flag
	cmp	bx,1			; test flag
	jne	clearDma		; jump if not set

	;-- setup for ppvBuffer[1] --
	add	edi,edx			; add to edi
	       
	;-- loop through buffer writing zeros --
clearDma:
	mov	DWORD PTR[edi],eax	; move zero into buffer
	add	edi,4			; increment buffer pointer

	mov	DWORD PTR[edi],eax	; move zero into buffer
	add	edi,4			; increment buffer pointer

	mov	DWORD PTR[edi],eax	; move zero into buffer
	add	edi,4			; increment buffer pointer

	mov	DWORD PTR[edi],eax	; move zero into buffer
	add	edi,4			; increment buffer pointer

	sub	ecx,4			; decrement counter
	jnz	clearDma		; jump if not zero

	POPALL

	ret				; that's all
ClearDmaBuf_   endp

;---------------------------------------------------------

;
; void MonoCopyNormal(void)
;
MonoCopyNormal_ proc near
	
	PUSHALL
	
	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

MonoCopyNormalLoop:
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to  ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	inc	esi			; increment source pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR[edi],edx	; write sample to mix buffer
	
	add	edi,8			; increment destination pointer
	dec	ecx			; decrement loop count
	jnz	MonoCopyNormalLoop

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
MonoCopyNormal_ endp

;
; void MonoCopyNormalHalf(void)
;
MonoCopyNormalHalf_ proc near
	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	inc	esi			; increment source pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisSampleG,edx	; store decompressed sample

	dec	ecx			; decrement loop count

MonoCopyNormalHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table pointer
	inc	esi			; increment source pointer

	;-- write this sample --
	mov	edx,dwThisSampleG	; get this sample
	mov	DWORD PTR[edi],edx	; write this sample to buffer
	add	edi,8			; increment destination pointer

	;-- get next sample --
	movsx	edx,WORD PTR[ebx] 	; get decompressed sample
	mov	dwNextSampleG,edx	; store next sample

	;-- calculate interpolation value --
	sub	edx,dwThisSampleG	; subtract this sample from next
	sar	edx,1			; divide by 2 to give interpolation
	add 	edx,dwThisSampleG	; add this sample

	;-- write this sample + interpolation --
	mov	DWORD PTR [edi],edx	; write next sample + interpol
	add	edi,8			; increment destination pointer

	mov	edx,dwNextSampleG	; get next sample
	mov	dwThisSampleG,edx	; store in this sample

	dec	ecx
	jnz	MonoCopyNormalHalfLoop

	;-- do last sample --
	mov	DWORD PTR [edi],edx	; write this sample to buffer
	add	edi,8			; increment buffer pointer
	mov	DWORD PTR [edi],edx	; wrote this sample to buffer
	add	edi,8			; increment buffer pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL
		
	ret
MonoCopyNormalHalf_ endp

	
;
; void MonoCopyPitch(void)
;
MonoCopyPitch_ proc near
	
	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

MonoCopyPitchLoop:
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR [edi],edx	; write sample to mix buffer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	add 	edi,8			; increment destination pointer
	dec	ecx			; decrement loop count
	jnz	MonoCopyPitchLoop

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
MonoCopyPitch_ endp

;
; void MonoCopyPitchHalf(void)
;
MonoCopyPitchHalf_ proc near

	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisSampleG,edx	; store decompressed sample

	;-- increment source pointer --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	ecx			; decrement loop count

MonoCopyPitchHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table pointer

	;-- write this sample --
	mov	edx,dwThisSampleG	; get this sample
	mov	DWORD PTR[edi],edx	; write this sample to buffer
	add	edi,8			; increment destination pointer

	;-- get next sample --
	movsx	edx,WORD PTR[ebx] 	; get decompressed sample
	mov	dwNextSampleG,edx	; store next sample

	;-- calculate interpolation value --
	sub	edx,dwThisSampleG	; subtract this sample from next
	sar	edx,1			; divide by 2 to give interpolation
	add 	edx,dwThisSampleG	; add this sample

	;-- write this sample + interpolation --
	mov	DWORD PTR [edi],edx	; write next sample + interpol
	add	edi,8			; increment destination pointer

	;-- increment source pointer --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	mov	edx,dwNextSampleG	; get next sample
	mov	dwThisSampleG,edx	; store in this sample

	dec	ecx
	jnz	MonoCopyPitchHalfLoop

	;-- do last sample --
	mov	DWORD PTR [edi],edx	; write this sample to buffer
	add	edi,8			; increment buffer pointer
	mov	DWORD PTR [edi],edx	; wrote this sample to buffer
	add	edi,8			; increment buffer pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL
		
	ret
MonoCopyPitchHalf_ endp

;
; void MonoMixNormal(void)
;
MonoMixNormal_ proc near

	PUSHALL
	
	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

MonoMixNormalLoop:
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to  ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	inc	esi			; increment source pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR[edi],edx	; add sample to mix buffer
	
	add	edi,8			; increment destination pointer
	dec	ecx			; decrement loop count
	jnz	MonoMixNormalLoop

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
MonoMixNormal_ endp

;
; void MonoMixNormalHalf(void)
;
MonoMixNormalHalf_ proc near
	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	inc	esi			; increment source pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisSampleG,edx	; store decompressed sample

	dec	ecx			; decrement loop count

MonoMixNormalHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table pointer
	inc	esi			; increment source pointer

	;-- write this sample --
	mov	edx,dwThisSampleG	; get this sample
	add	DWORD PTR[edi],edx	; write this sample to buffer
	add	edi,8			; increment destination pointer

	;-- get next sample --
	movsx	edx,WORD PTR[ebx] 	; get decompressed sample
	mov	dwNextSampleG,edx	; store next sample

	;-- calculate interpolation value --
	sub	edx,dwThisSampleG	; subtract this sample from next
	sar	edx,1			; divide by 2 to give interpolation
	add 	edx,dwThisSampleG	; add this sample

	;-- write this sample + interpolation --
	add	DWORD PTR [edi],edx	; write next sample + interpol
	add	edi,8			; increment destination pointer

	mov	edx,dwNextSampleG	; get next sample
	mov	dwThisSampleG,edx	; store in this sample

	dec	ecx
	jnz	MonoMixNormalHalfLoop

	;-- do last sample --
	add	DWORD PTR [edi],edx	; write this sample to buffer
	add	edi,8			; increment buffer pointer
	add	DWORD PTR [edi],edx	; wrote this sample to buffer
	add	edi,8			; increment buffer pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL
		
	ret
MonoMixNormalHalf_ endp

;
; void MonoMixPitch(void)
;
MonoMixPitch_ proc near

	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

MonoMixPitchLoop:
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR [edi],edx	; write sample to mix buffer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	add 	edi,8			; increment destination pointer
	dec	ecx			; decrement loop count
	jnz	MonoMixPitchLoop

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
MonoMixPitch_ endp

;
; void MonoMixPitchHalf(void)
;
MonoMixPitchHalf_ proc near

	PUSHALL

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table base pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisSampleG,edx	; store decompressed sample

	;-- increment source pointer --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	ecx			; decrement loop count

MonoMixPitchHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get sample to give offset
	mov	ebx,eax			; copy offset to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add lookup table pointer

	;-- write this sample --
	mov	edx,dwThisSampleG	; get this sample
	add	DWORD PTR[edi],edx	; write this sample to buffer
	add	edi,8			; increment destination pointer

	;-- get next sample --
	movsx	edx,WORD PTR[ebx] 	; get decompressed sample
	mov	dwNextSampleG,edx	; store next sample

	;-- calculate interpolation value --
	sub	edx,dwThisSampleG	; subtract this sample from next
	sar	edx,1			; divide by 2 to give interpolation
	add 	edx,dwThisSampleG	; add this sample

	;-- write this sample + interpolation --
	add	DWORD PTR [edi],edx	; write next sample + interpol
	add	edi,8			; increment destination pointer

	;-- increment source pointer --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	mov	edx,dwNextSampleG	; get next sample
	mov	dwThisSampleG,edx	; store in this sample

	dec	ecx
	jnz	MonoMixPitchHalfLoop

	;-- do last sample --
	add	DWORD PTR [edi],edx	; write this sample to buffer
	add	edi,8			; increment buffer pointer
	add	DWORD PTR [edi],edx	; wrote this sample to buffer
	add	edi,8			; increment buffer pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL
		
	ret
MonoMixPitchHalf_ endp

;
; void MonoZeroPad(void)
; 
MonoZeroPad_ proc near

	PUSHALL

	;-- setup registers --
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG

MonoZeroPadLoop:
	mov	DWORD PTR [edi],0	; copy zero to mix buffer
	add	edi,8			; increment destination pointer

	dec	ecx			; decrement loop count
	jnz	MonoZeroPadLoop		

	;-- update pointer --
	mov	_pdwDstPtrG,edi

	POPALL

	ret
MonoZeroPad_ endp

;
; void StereoCopyNormal(void)
;
StereoCopyNormal_ proc near
	
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	;mov	ecx,_dwLoopCountG
	mov	cl,_bLeftOffG
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

StereoCopyNormalLoop:
	mov	al, BYTE PTR[esi]	; get compressed sample

	;-- decompress left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- decompress right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	
	inc 	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoCopyNormalLoop
	
	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi
	
	POPALL

	ret
StereoCopyNormal_ endp

;
; void StereoCopyNormalHalf(void)
;
StereoCopyNormalHalf_ proc near
	PUSHALL	

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	cl,_bLeftOffG	       ; pack cx with L&R offsets
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

	mov	al, BYTE PTR [esi]	; get compressed sample

	;-- get first left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisLeftSampleG,edx   ; store left sample
	mov	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- get first right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisRightSampleG,edx  ; store right sample
	mov	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	
	
	inc	esi			; increment source pointer

	dec 	_dwLoopCountG		; decrement loop counter

StereoCopyNormalHalfLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- get next left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextLeftSampleG,edx   ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisLeftSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisLeftSampleG	; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- get next right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextRightSampleG,edx  ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisRightSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisRightSampleG	; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- swap left samples and write --
	mov	edx,dwNextLeftSampleG	; get next left sample
	mov	dwThisLeftSampleG,edx	; store as this left sample
	mov	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- swap right samples and write --
	mov	edx,dwNextRightSampleG	; get next right sample
	mov	dwThisRightSampleG,edx	; store as this right sample
	mov	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop count
	jnz	StereoCopyNormalHalfLoop

	;-- do the last samples --
	mov	edx,dwThisLeftSampleG	; get this left sample
	mov	ecx,dwThisRightSampleG 	; get this right sample
	mov	DWORD PTR [edi],edx	; write left sample to buffer
	add	edi,4			; increment pointer
	mov	DWORD PTR [edi],ecx	; write right sample to buffer
	add	edi,4			; increment pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyNormalHalf_ endp

;
; void StereoCopyPitch(void)
;	
StereoCopyPitch_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	cl,_bLeftOffG
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

StereoCopyPitchLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- decompress left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- decompress right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum
		
	dec 	_dwLoopCountG		; decrement loop count
	jnz	StereoCopyPitchLoop	

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyPitch_ endp

;
; void StereoCopyPitchHalf(void)
;
StereoCopyPitchHalf_ proc near
	PUSHALL	

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	cl,_bLeftOffG	       ; pack cx with L&R offsets
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

	mov	al, BYTE PTR [esi]	; get compressed sample

	;-- get first left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisLeftSampleG,edx   ; store left sample
	mov	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- get first right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisRightSampleG,edx  ; store right sample
	mov	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec 	_dwLoopCountG		; decrement loop counter

StereoCopyPitchHalfLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- get next left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextLeftSampleG,edx   ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisLeftSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisLeftSampleG	; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- get next right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextRightSampleG,edx  ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisRightSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisRightSampleG	; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- swap left samples and write --
	mov	edx,dwNextLeftSampleG	; get next left sample
	mov	dwThisLeftSampleG,edx	; store as this left sample
	mov	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- swap right samples and write --
	mov	edx,dwNextRightSampleG	; get next right sample
	mov	dwThisRightSampleG,edx	; store as this right sample
	mov	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop count
	jnz	StereoCopyPitchHalfLoop

	;-- do the last samples --
	mov	edx,dwThisLeftSampleG	; get this left sample
	mov	ecx,dwThisRightSampleG 	; get this right sample
	mov	DWORD PTR [edi],edx	; write left sample to buffer
	add	edi,4			; increment pointer
	mov	DWORD PTR [edi],ecx	; write right sample to buffer
	add	edi,4			; increment pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyPitchHalf_ endp

;
; void StereoMixNormal(void)
;
StereoMixNormal_ proc near
	
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	cl,_bLeftOffG
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

StereoMixNormalLoop:
	mov	al, BYTE PTR[esi]	; get compressed sample

	;-- decompress left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR [edi],edx	; add to mix buffer
	add	edi,4			; increment destination pointer

	;-- decompress right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR [edi],edx	; add to mix buffer
	add	edi,4			; increment destination pointer
	
	inc 	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoMixNormalLoop
	
	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi
	
	POPALL

	ret
StereoMixNormal_ endp

;
; void StereoMixNormalHalf(void)
;
StereoMixNormalHalf_ proc near
	PUSHALL	

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	cl,_bLeftOffG	       ; pack cx with L&R offsets
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

	mov	al, BYTE PTR [esi]	; get compressed sample

	;-- get first left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisLeftSampleG,edx   ; store left sample
	add	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- get first right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisRightSampleG,edx  ; store right sample
	add	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	
	
	inc	esi			; increment source pointer

	dec 	_dwLoopCountG		; decrement loop counter

StereoMixNormalHalfLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- get next left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextLeftSampleG,edx   ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisLeftSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisLeftSampleG	; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- get next right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextRightSampleG,edx  ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisRightSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisRightSampleG	; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- swap left samples and write --
	mov	edx,dwNextLeftSampleG	; get next left sample
	mov	dwThisLeftSampleG,edx	; store as this left sample
	add	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- swap right samples and write --
	mov	edx,dwNextRightSampleG	; get next right sample
	mov	dwThisRightSampleG,edx	; store as this right sample
	add	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop count
	jnz	StereoMixNormalHalfLoop

	;-- do the last samples --
	mov	edx,dwThisLeftSampleG	; get this left sample
	mov	ecx,dwThisRightSampleG 	; get this right sample
	add	DWORD PTR [edi],edx	; write left sample to buffer
	add	edi,4			; increment pointer
	add	DWORD PTR [edi],ecx	; write right sample to buffer
	add	edi,4			; increment pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixNormalHalf_ endp


;
; void StereoMixPitch(void)
;	
StereoMixPitch_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	cl,_bLeftOffG
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

StereoMixPitchLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- decompress left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- decompress right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum
		
	dec 	_dwLoopCountG		; decrement loop count
	jnz	StereoMixPitchLoop	

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixPitch_ endp

;
; void StereoMixPitchHalf(void)
;
StereoMixPitchHalf_ proc near
	PUSHALL	

	;-- setup registers --
	mov 	esi,_pbSrcPtrG		
	mov	edi,_pdwDstPtrG
	xor	eax,eax
	mov	ax,_wOffsetG
	mov	cl,_bLeftOffG	       ; pack cx with L&R offsets
	mov	ch,_bRightOffG
	mov	ebp,_pwDeCmprsG

	mov	al, BYTE PTR [esi]	; get compressed sample

	;-- get first left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisLeftSampleG,edx   ; store left sample
	add	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- get first right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwThisRightSampleG,edx  ; store right sample
	add	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec 	_dwLoopCountG		; decrement loop counter

StereoMixPitchHalfLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- get next left sample --
	mov	ah,cl			; get left offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextLeftSampleG,edx   ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisLeftSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisLeftSampleG	; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- get next right sample --
	mov	ah,ch			; get right offset
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextRightSampleG,edx  ; store left sample

	;-- calculate interpolated sample --
	sub	edx,dwThisRightSampleG	; subtract this sample
	sar	edx,1			; divide by 2 to give interpolation
	add	edx,dwThisRightSampleG	; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- swap left samples and write --
	mov	edx,dwNextLeftSampleG	; get next left sample
	mov	dwThisLeftSampleG,edx	; store as this left sample
	add	DWORD PTR [edi],edx	; write left sample
	add	edi,4			; increment pointer	

	;-- swap right samples and write --
	mov	edx,dwNextRightSampleG	; get next right sample
	mov	dwThisRightSampleG,edx	; store as this right sample
	add	DWORD PTR [edi],edx	; write right sample
	add	edi,4			; increment pointer	

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop count
	jnz	StereoMixPitchHalfLoop

	;-- do the last samples --
	mov	edx,dwThisLeftSampleG	; get this left sample
	mov	ecx,dwThisRightSampleG 	; get this right sample
	add	DWORD PTR [edi],edx	; write left sample to buffer
	add	edi,4			; increment pointer
	add	DWORD PTR [edi],ecx	; write right sample to buffer
	add	edi,4			; increment pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixPitchHalf_ endp


;
;-- void StereoCopyNormalSurround(void)
;
StereoCopyNormalSurround_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

StereoCopyNormalSurroundLoop:
	mov	al, BYTE PTR[esi]	; get compressed sample

	;-- decompress left sample --
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	
	;-- invert sample --
	neg	edx			; invert
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	inc 	esi			; increment source pointer
	dec	ecx			; decrement loop counter
	jnz	StereoCopyNormalSurroundLoop
	
	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi
	
	POPALL
		
	ret
StereoCopyNormalSurround_ endp	

;
; void StereoCopyNormalSurroundHalf(void)
;
StereoCopyNormalSurroundHalf_ proc near
	
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	ecx,edx			; store this sample in ecx
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert edx
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop count

StereoCopyNormalSurroundHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextSampleG,edx	; store as next sample

	;-- calculate interpolated sample --
	sub 	edx,ecx			; subtract this sample from next
	sar	edx,1			; divide by two
	add	edx,ecx			; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer

	;-- swap samples --
	mov	edx,dwNextSampleG	; get next sample
	mov	ecx,edx			; store as this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer
	neg	edx			; invert sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoCopyNormalSurroundHalfLoop

	;-- do the last samples --
	mov	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer
	neg	ecx			; invert sample
	mov	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyNormalSurroundHalf_ endp

	
	
	

;
; void StereoCopyPitchSurround(void)
;
StereoCopyPitchSurround_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

StereoCopyPitchSurroundLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- decompress left sample --
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- invert sample --
	neg	edx			; invert
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	
	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum
		
	dec 	ecx			; decrement loop count
	jnz	StereoCopyPitchSurroundLoop	

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyPitchSurround_ endp

;
; void StereoCopyPitchSurroundHalf(void)
;
StereoCopyPitchSurroundHalf_ proc near
	
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	ecx,edx			; store this sample in ecx
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert edx
	mov	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop count

StereoCopyPitchSurroundHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextSampleG,edx	; store as next sample

	;-- calculate interpolated sample --
	sub 	edx,ecx			; subtract this sample from next
	sar	edx,1			; divide by two
	add	edx,ecx			; add this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer

	;-- swap samples --
	mov	edx,dwNextSampleG	; get next sample
	mov	ecx,edx			; store as this sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer
	neg	edx			; invert sample
	mov	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoCopyPitchSurroundHalfLoop

	;-- do the last samples --
	mov	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer
	neg	ecx			; invert sample
	mov	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoCopyPitchSurroundHalf_ endp	

;
; void StereoMixNormalSurround(void)
;
StereoMixNormalSurround_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

StereoMixNormalSurroundLoop:
	mov	al, BYTE PTR[esi]	; get compressed sample

	;-- decompress left sample --
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	
	;-- invert sample --
	sub	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	inc 	esi			; increment source pointer
	dec	ecx			; decrement loop counter
	jnz	StereoMixNormalSurroundLoop
	
	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi
	
	POPALL
		
	ret
StereoMixNormalSurround_ endp	

;
; void StereoMixNormalSurroundHalf(void)
;
StereoMixNormalSurroundHalf_ proc near
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	ecx,edx			; store this sample in ecx
	add	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert edx
	add	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop count

StereoMixNormalSurroundHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextSampleG,edx	; store as next sample

	;-- calculate interpolated sample --
	sub 	edx,ecx			; subtract this sample from next
	sar	edx,1			; divide by two
	add	edx,ecx			; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer

	;-- swap samples --
	mov	edx,dwNextSampleG	; get next sample
	mov	ecx,edx			; store as this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer
	neg	edx			; invert sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	inc	esi			; increment source pointer
	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoMixNormalSurroundHalfLoop

	;-- do the last samples --
	add	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer
	neg	ecx			; invert sample
	add	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixNormalSurroundHalf_ endp

;
; void StereoMixPitchSurround(void)
;
StereoMixPitchSurround_ proc near

	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

StereoMixPitchSurroundLoop:
	mov	al,BYTE PTR [esi]	; get compressed sample

	;-- decompress left sample --
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by two
	add	ebx,ebp			; add decompression table pointer
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	add	DWORD PTR[edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- invert sample --
	sub	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	
	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum
		
	dec 	ecx			; decrement loop count
	jnz	StereoMixPitchSurroundLoop	

	;-- update pointers --
	mov	_pbSrcPtrG,esi
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixPitchSurround_ endp

;
; void StereoMixPitchSurroundHalf(void)
;
StereoMixPitchSurroundHalf_ proc near
	
	PUSHALL

	;-- setup registers --
	xor	eax,eax
	mov	esi,_pbSrcPtrG
	mov	edi,_pdwDstPtrG
	mov	ebp,_pwDeCmprsG
	mov	ah,_bLeftOffG

	;-- get first sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	ecx,edx			; store this sample in ecx
	add	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert edx
	add	DWORD PTR [edi],edx	; copy to mix buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop count

StereoMixPitchSurroundHalfLoop:
	;-- get next sample --
	mov	al,BYTE PTR [esi]	; get compressed sample
	mov	ebx,eax			; copy to ebx
	add	ebx,ebx			; multiply by 2
	add	ebx,ebp			; add decompression table address
	movsx	edx,WORD PTR [ebx]	; get decompressed sample
	mov	dwNextSampleG,edx	; store as next sample

	;-- calculate interpolated sample --
	sub 	edx,ecx			; subtract this sample from next
	sar	edx,1			; divide by two
	add	edx,ecx			; add this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer
	neg	edx			; invert sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer

	;-- swap samples --
	mov	edx,dwNextSampleG	; get next sample
	mov	ecx,edx			; store as this sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add 	edi,4			; increment destination pointer
	neg	edx			; invert sample
	add	DWORD PTR [edi],edx	; write sample to buffer
	add	edi,4			; increment destination pointer

	;-- calculate pitch shift increment --
	mov	bx,_wIncIndxG		; get increment sum
	add	bx,_wIncrementG		; add increment 
	xor	edx,edx			; clear edx
	mov	dl,bh			; get integer component
	add	esi,edx			; add to sample pointer
	mov	bh,0			; clear integer component
	mov	_wIncIndxG,bx		; store increment sum

	dec	_dwLoopCountG		; decrement loop counter
	jnz	StereoMixPitchSurroundHalfLoop

	;-- do the last samples --
	add	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer
	neg	ecx			; invert sample
	add	DWORD PTR [edi],ecx	; write this sample
	add	edi,4			; increment destination pointer

	;-- update pointers --
	mov	_pbSrcPtrG,esi		
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoMixPitchSurroundHalf_ endp	


;
; void StereoZeroPad(void)
;
StereoZeroPad_ proc near
	
	PUSHALL

	;-- setup registers --
	mov	edi,_pdwDstPtrG
	mov	ecx,_dwLoopCountG

StereoZeroPadLoop:
	mov	DWORD PTR [edi],0	; copy zero to mix buffer
	add	edi,4			; increment destination pointer

	mov	DWORD PTR [edi],0	; copy zero to mix buffer
	add	edi,4			; increment destination pointer

	dec	ecx			; decrement loop count
	jnz	StereoZeroPadLoop		

	;-- update pointer --
	mov	_pdwDstPtrG,edi

	POPALL

	ret
StereoZeroPad_ endp


;----------------------------------------------------------

Mix2Dma16Mono_ proc near
	;extern uint32 dwDmaBlockSamplesG;
	;extern void *ppvBufferG[2];
	;extern volatile uint16 wBufferFlagG;
	;extern uint32 dwDmaBlockSizeG;
	;extern int32 *pdwMixBufG;

	PUSHALL

	mov	ecx,_dwDmaBlockSamplesG		; copy buffer length to counter
	mov	edi,_ppvBufferG			; get buffer pointer
			 
	;-- test wBufferFlag --
	mov	dx,_wBufferFlagG	    	; get buffer flag
	test	dx,1
	jz	@@dmaCopy

	;-- set up for ppvBuffer[1] --
	add	edi,_dwDmaBlockSizeG		; ebx = ppwBuffer[1]

@@dmaCopy:		     
	;== Dma buffer loop ==		  
	mov 	esi,_pdwMixBufG			; eax = pMixBuf	  			   
@@finalLoop:
	mov 	eax,DWORD PTR [esi]  		; get left sample
	mov	WORD PTR [edi],ax		; store in DMA buffer left

	add	eax,32768			; test sample for 16bit overflow
	cmp	eax,65536			; if occurred, then jump to
	jnc	@@overflow			; clip the sample
@@sampleOk:

    	add	esi,8				; increment mix buffer pointer
	add 	edi,2				; increment DMA buffer pointer

	;== repeat for optimisation ==
	mov 	eax,DWORD PTR [esi]  		; get left sample
	mov	WORD PTR [edi],ax		; store in DMA buffer left

	add	eax,32768			; test sample for 16bit overflow
	cmp	eax,65536			; if occurred, then jump to
	jnc	@@overflow2			; clip the sample
@@sampleOk2:

    	add	esi,8				; increment mix buffer pointer
	add 	edi,2				; increment DMA buffer pointer

	sub 	ecx,2				; decrement counter
	jnz	@@finalLoop			; jump if counter not zero

	POPALL

	ret					; return

	;-- clip overflow sample to 16bits --
@@overflow:
	or	eax,eax				; set flags for edx
	js	@@edx_signed			; if signed data, ie negative, jump

	mov	WORD PTR [edi],32767		; clip to 32767
	jmp	@@sampleOk			; jump back
@@edx_signed:
	mov	WORD PTR [edi],-32768		; clip to -32768
	jmp	@@sampleOk			; jump back

@@overflow2:
	or	eax,eax				; set flags for edx
	js	@@edx_signed2			; if signed data, ie negative, jump

	mov	WORD PTR [edi],32767		; clip to 32767
	jmp	@@sampleOk2			; jump back
@@edx_signed2:
	mov	WORD PTR [edi],-32768		; clip to -32768
	jmp	@@sampleOk2			; jump back


Mix2Dma16Mono_ endp



Mix2Dma16Stereo_ proc near
	;extern uint32 dwMixBufFramesG;
	;extern void *ppvBufferG[2];
	;extern uint32 dwDmaBlockSizeG;
	;extern uint32 *pdwMixBufG;	
	;extern volatile uint16 wBufferFlagG;

	PUSHALL

	mov	ecx,_dwMixBufFramesG		; copy buffer length to counter
	mov	edi,_ppvBufferG			; get buffer pointer

	;-- test wBufferFlag --
	mov	dx,_wBufferFlagG	       	; get buffer flag
	test	dx,1
	jz	@@dmaCopy

	;-- set up for ppvBuffer[1] --
	add	edi,_dwDmaBlockSizeG		; ebx = ppwBuffer[1]
@@dmaCopy:		    
 
	;== Dma buffer loop ==	        
	mov 	esi,_pdwMixBufG			; eax = pMixBuf				   
@@finalLoop:

	;-- copy and clip left sample --
	mov 	eax,DWORD PTR [esi]  		; get left sample
	mov	WORD PTR [edi],ax		; store in DMA buffer left

	add	eax,32768			; test sample for 16bit overflow
	cmp	eax,65536			; if occurred, then jump to
	jnc	@@leftOverflow			; clip the sample
@@leftOk:
    	add	esi,4				; increment mix buffer pointer
	add 	edi,2				; increment dma buffer pointer


	;-- copy and clip right sample --
	mov 	eax,DWORD PTR [esi]		; get right sample
	mov	WORD PTR [edi],ax		; store in DMA buffer right
	
	add 	eax,32768			; test sample for 16bit overflow
	cmp	eax,65536			; if occured then jump to
	jnc	@@rightOverflow			; clip the sample
@@rightOk:
	add	esi,4				; increment mix buffer pointer
	add 	edi,2				; increment DMA buffer pointer

	dec 	ecx				; decrement counter
	jnz	@@finalLoop			; jump if counter not zero

	POPALL

	ret					; return

	;-- clip overflow sample to 16bits --
@@leftOverflow:
	or	eax,eax				; set flags for edx
	js	@@leftSigned			; if signed data, ie negative, jump

	mov	WORD PTR [edi],32767		; clip to 32767
	jmp	@@leftOk			; jump back
@@leftSigned:
	mov	WORD PTR [edi],-32768		; clip to -32768
	jmp	@@leftOk			; jump back

@@rightOverflow:
	or	eax,eax				; set flags for edx
	js	@@rightSigned			; if signed data, ie negative, jump

	mov	WORD PTR [edi],32767		; clip to 32767
	jmp	@@rightOk			; jump back
@@rightSigned:
	mov	WORD PTR [edi],-32768		; clip to -32768
	jmp	@@rightOk			; jump back

Mix2Dma16Stereo_ endp			 




Mix2Dma8Mono_ proc near
	;extern uint32 dwDmaBlockSamplesG;
	;extern void *ppvBufferG[2];
	;extern volatile uint16 wBufferFlagG;
	;extern uint32 dwDmaBlockSizeG;
	;extern int32 *pdwMixBufG;

	PUSHALL

	mov	ecx,_dwDmaBlockSamplesG		; copy buffer length to counter
	mov	edi,_ppvBufferG			; get buffer pointer
			 
	;-- test wBufferFlag --
	mov	dx,_wBufferFlagG		; get buffer flag
	test	dx,1
	jz	@@dmaCopy

	;-- set up for ppvBuffer[1] --
	add	edi,_dwDmaBlockSizeG		; ebx = ppwBuffer[1]

@@dmaCopy:		     
	;== Dma buffer loop ==		  
	mov 	esi,_pdwMixBufG			; eax = pMixBuf	  			   
@@finalLoop:
	mov 	eax,DWORD PTR [esi]  		; get left sample
	add	eax,128				; add 128 for 8bit unsigned

	cmp	eax,256				; if occurred, then jump to
	jnc	@@overflow			; clip the sample
@@sampleOk:

	mov	BYTE PTR [edi],al		; store in DMA buffer left

       	add	esi,8				; increment mix buffer pointer
	inc	edi				; increment DMA buffer pointer

	;== repeat loop for optimisation ==
	mov	eax,DWORD PTR [esi]		; get left sample
	add	eax,128				; add 128 for 8bit unsigned

	cmp	eax,256				; if occured, then jump to
	jnc	@@overflow2			; clip the sample
@@sampleOk2:

	mov	BYTE PTR [edi],al		; store in DMA buffer

	add	esi,8				; increment mix buffer pointer
	inc	edi				; increment DMA buffer pointer

	sub 	ecx,2				; decrement counter
	jnz	@@finalLoop			; jump if counter not zero

	POPALL

	ret					; return

	;-- clip overflow sample to 8bits --
@@overflow:
	or	eax,eax				; set flags for edx
	js	@@edx_signed			; if signed data, ie negative, jump

	mov	ax,255				; re-write left sample
	jmp	@@sampleOk			; jump back
@@edx_signed:
	mov	ax,0				; re-write left sample
	jmp	@@sampleOk			; jump back

@@overflow2:
	or	eax,eax				; set flags for edx
	js	@@edx_signed2			; if signed data, ie negative, jump

	mov	ax,255				; re-write left sample
	jmp	@@sampleOk2			; jump back
@@edx_signed2:
	mov	ax,0				; re-write left sample
	jmp	@@sampleOk2			; jump back

Mix2Dma8Mono_ endp
				   
Mix2Dma8Stereo_ proc near
	;extern uint32 dwMixBufFramesG;
	;extern void *ppvBufferG[2];
	;extern volatile uint16 wBufferFlagG;
	;extern uint32 dwDmaBlockSizeG;
	;extern int32 *pdwMixBufG;

	PUSHALL

	mov 	ecx,_dwMixBufFramesG		; copy buffer length to counter
	mov	edi,_ppvBufferG			; get buffer pointer
			 
	;-- test wBufferFlag --
	mov	dx,_wBufferFlagG		; get buffer flag
	test	dx,1
	jz	@@dmaCopy

	;-- set up for ppvBuffer[1] --
	add	edi,_dwDmaBlockSizeG		; ebx = ppwBuffer[1]

@@dmaCopy:		     	     
	;== Dma buffer loop ==		  
	mov 	esi,_pdwMixBufG			; esi = pMixBuf	  			   
@@finalLoop:
	;-- get left sample --
	mov 	eax,DWORD PTR [esi]  		; get left sample
	add	eax,128				; converted to 8bit unsigned

	;-- check for overflow --
	cmp	eax,256				; if occurred, then jump to
	jnc	@@leftOverflow			; clip the sample
@@leftOk:
	mov	BYTE PTR [edi],al;		; store in DMA buffer left

	add	esi,4				; increment source pointer
	inc	edi				; increment destination pointer

	;-- get right sample --
	mov	eax,DWORD PTR [esi]		; get right sample
	add	eax,128				; convert to 8bit unsigned

	;-- check for overflow --
	cmp	eax,256				; if occurred, then jump to
	jnc	@@rightOverflow			; clip the sample
@@rightOk:
	mov	BYTE PTR [edi],al		; store in DMA buffer right

    	add	esi,4				; increment mix buffer pointer
	inc	edi				; increment DMA buffer pointer

	dec 	ecx				; decrement counter
	jnz	@@finalLoop			; jump if counter not zero

	POPALL

	ret					; return
		  
	;-- clip overflow sample to 8bits --
@@leftOverflow:
	or	eax,eax				; set flags for edx
	js	@@leftSigned			; if signed data, ie negative, jump

	mov	ax,255				; re-write left sample
	jmp	@@leftOk			; jump back
@@leftSigned:
	mov	ax,0				; re-write left sample
	jmp	@@leftOk			; jump back

@@rightOverflow:
	or	eax,eax				; set flags for edx
	js	@@rightSigned			; if signed data, ie negative, jump

	mov	ax,255				; re-write left sample
	jmp	@@rightOk			; jump back
@@rightSigned:
	mov	ax,0				; re-write left sample
	jmp	@@rightOk			; jump back
	


Mix2Dma8Stereo_ endp			        

	END32CODE

	end
 
    