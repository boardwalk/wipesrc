
	include flat32.i

	public MonoEcho_
	public StereoEcho_
	public MonoDelay_
	public StereoDelay_
	public MonoReverb_
	public StereoReverb_

	START32DATA

	extern _pdwMixBufG	   		:dword		; pointer to mix buffer
	extern _dwMixBufFramesG	   	:dword		; number of frames in mix buffer
	extern _bDepthG		   	:byte		; depth value
	extern _bDelayCoeffG		:byte		; delay feedback coeff.
	extern _pdwWorkBufG			:dword		; pointer to work buffer

	extern _pdwDelayPtrG	   	:dword 		; pointer to Delay buffer
	extern _pdwDelayPosG	   	:dword 		; current position in Delay buffer
	extern _dwDelayLenG	   		:dword 		; length of delay buffer

	extern _pdwComb1PtrG		:dword
	extern _pdwComb1PosG		:dword
	extern _dwComb1LenG			:dword

	extern _pdwComb2PtrG		:dword
	extern _pdwComb2PosG		:dword
	extern _dwComb2LenG			:dword

	extern _pdwComb3PtrG		:dword
	extern _pdwComb3PosG		:dword
	extern _dwComb3LenG			:dword

	extern _pdwAllPassPtrG		:dword
	extern _pdwAllPassPosG		:dword
	extern _dwAllPassLenG		:dword

	;-- local globals --
	dwMixBufNotDone 		dd	?
	dwDelayBufNotDone		dd	?
	dwLoopCount			dd	?
	dwInputSample			dd 	?
	dwBufEnd				dd   ?
	dwEnd1Ptr				dd	?
	dwEnd2Ptr				dd   ?
	dwEnd3Ptr				dd   ?


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
; void MonoEcho(void)
;
MonoEcho_ proc near
	PUSHALL

	;-- setup registers --
	mov	edi,_pdwDelayPosG	; set edi to delay pointer
	mov	esi,_pdwMixBufG		; set esi for mix pointer
	mov	ebp,_pdwDelayPtrG;	; set ebp to delay buffer start
	xor	ecx,ecx			; clear ecx
	mov	cl,_bDepthG		; get depth

	mov	edx,_dwMixBufFramesG	; edx = dwMixBufNotDone
	mov	dwMixBufNotDone,edx	; store
	
MonoEchoOuterLoop:
	mov	edx,dwMixBufNotDone;	; get num samples left
	or	edx,edx			; test edx
	jz	MonoEchoEnd		; jump to end if all done

	;-- calculate delay buffer not done --
	mov	eax,edi			; copy delay current position
	sub	eax,ebp			; subtract delay start
	sar	eax,2			; divide by 4 for bytes
	mov	ebx,_dwDelayLenG	; get delay length
	sub 	ebx,eax			; get delay buf not done
	mov	dwDelayBufNotDone,ebx 	; store

	;-- calculate loop count --
	cmp	edx,ebx			; compare remaining mix and delay buffers
	jg	MonoEchoDelay
	mov	eax,edx			; loop is mix buffer not done
	jmp	MonoEchoSub
MonoEchoDelay:
	mov	eax,ebx			; loop is delay buffer not done
MonoEchoSub:
	
	;-- subtract loop count from not dones --
	sub	ebx,eax			; dwDelayBufNotDone - loop
	sub	edx,eax			; dwMixBufNotDone - loop
	mov	dwDelayBufNotDone,ebx	; store
	mov	dwMixBufNotDone,edx	; store

	mov	ebx,eax			; mov loop to ebx

	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get next left input sample
	mov	DWORD PTR[edi],eax	; write to input buffer

	add	edi,4			; increment delay pointer
	
	dec	ebx			; decrement loop count
	or	ebx,ebx			; test ebx
	jz	MonoEchoLoopEnd	; jump if loop count == 0

MonoEchoLoop:
	;-- output delayed samples --
	mov	eax,DWORD PTR [edi]    	; get output sample
	sar	eax,cl			; shift right by depth
	add	DWORD PTR[esi],eax	; add to mix buffer
	add	esi,8			; increment mix buf pointer

	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get next left input sample
	mov	DWORD PTR[edi],eax	; write to input buffer

	add	edi,4			; increment delay pointer
	dec	ebx			; decrement loop count
	jnz	MonoEchoLoop		; loop if not zero

MonoEchoLoopEnd:

	;-- reset delay pointer if reached end --
	mov	edx,dwDelayBufNotDone	; get delay buf remainder
	or	edx,edx			; test edx
	jnz	MonoEchoLastOut		; jump if not zero
	mov	edi,ebp			; reset pointer to start
	
MonoEchoLastOut:
	;-- output delayed samples --
	mov	eax,DWORD PTR [edi]    	; get output sample
	sar	eax,cl			; shift right by depth
	add	DWORD PTR[esi],eax	; add to mix buffer
	add	esi,8			; increment mix buf pointer

	jmp    	MonoEchoOuterLoop	; end of while loop

MonoEchoEnd:
	mov	_pdwDelayPosG,edi

	POPALL

	ret
MonoEcho_ endp


;
; void StereoEcho(void)
;
StereoEcho_ proc near

	PUSHALL

	;-- setup registers --
	mov	edi,_pdwDelayPosG	; set edi to delay pointer
	mov	esi,_pdwMixBufG		; set esi for mix pointer
	mov	ebp,_pdwDelayPtrG;	; set ebp to delay buffer start
	xor	ecx,ecx			; clear ecx
	mov	cl,_bDepthG		; get depth

	mov	edx,_dwMixBufFramesG	; edx = dwMixBufNotDone
	mov	dwMixBufNotDone,edx	; store
	
StereoEchoOuterLoop:
	mov	edx,dwMixBufNotDone;	; get num samples left
	or	edx,edx			; test edx
	jz	StereoEchoEnd		; jump to end if all done

	;-- calculate delay buffer not done --
	mov	eax,edi			; copy delay current position
	sub	eax,ebp			; subtract delay start
	sar	eax,2			; divide by 4 for bytes
	mov	ebx,_dwDelayLenG	; get delay length
	sub 	ebx,eax			; get delay buf not done
	mov	dwDelayBufNotDone,ebx 	; store

	;-- calculate loop count --
	cmp	edx,ebx			; compare remaining mix and delay buffers
	jg	StereoEchoDelay
	mov	eax,edx			; loop is mix buffer not done
	jmp	StereoEchoSub
StereoEchoDelay:
	mov	eax,ebx			; loop is delay buffer not done
StereoEchoSub:
	
	;-- subtract loop count from not dones --
	sub	ebx,eax			; dwDelayBufNotDone - loop
	sub	edx,eax			; dwMixBufNotDone - loop
	mov	dwDelayBufNotDone,ebx	; store
	mov	dwMixBufNotDone,edx	; store

	mov	ebx,eax			; mov loop to ebx

	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get next left input sample
	add	eax,DWORD PTR[esi+4]	; get next right input sample
	sar	eax,1			; divide by 2
	mov	DWORD PTR[edi],eax	; write to input buffer

	add	edi,4			; increment delay pointer
	
	dec	ebx			; decrement loop count
	or	ebx,ebx			; test ebx
	jz	StereoEchoLoopEnd	; jump if loop count == 0

StereoEchoLoop:
	;-- output delayed samples --
	mov	eax,DWORD PTR [edi]    	; get output sample
	sar	eax,cl			; shift right by depth
	add	DWORD PTR[esi],eax	; add to mix buffer
	add	esi,4			; increment mix buf pointer
	sub	DWORD PTR[esi],eax	; subtract from mix buffer
	add	esi,4			; increment mix buf pointer

	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get next left input sample
	add	eax,DWORD PTR[esi+4]	; get next right input sample
	sar	eax,1			; divide by 2
	mov	DWORD PTR[edi],eax	; write to input buffer

	add	edi,4			; increment delay pointer
	dec	ebx			; decrement loop count
	jnz	StereoEchoLoop		; loop if not zero

StereoEchoLoopEnd:

	;-- reset delay pointer if reached end --
	mov	edx,dwDelayBufNotDone	; get delay buf remainder
	or	edx,edx			; test edx
	jnz	StereoEchoLastOut	; jump if not zero
	mov	edi,ebp			; reset pointer to start
	
StereoEchoLastOut:
	;-- output delayed samples --
	mov	eax,DWORD PTR [edi]    	; get output sample
	sar	eax,cl			; shift right by depth
	add	DWORD PTR[esi],eax	; add to mix buffer
	add	esi,4			; increment mix buf pointer
	sub	DWORD PTR[esi],eax	; subtract from mix buffer
	add	esi,4			; increment mix buf pointer

	jmp	StereoEchoOuterLoop	; end of while loop

StereoEchoEnd:
	;mov	_pdwMixPtrG,esi
	mov	_pdwDelayPosG,edi

	POPALL

	ret
StereoEcho_ endp	

;
; void MonoDelay(void)
;
MonoDelay_ proc near

	PUSHALL

	;-- setup registers --
	mov	esi,_pdwMixBufG		; pointer to mix buffer
	mov	edi,_pdwDelayPosG	; current position in delay buffer

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store 
	xor	ecx,ecx			; clear ecx
	mov	cl,_bDepthG		; depth value
	mov	ch,_bDelayCoeffG	; delay feedback coeff.

	;-- calculate end of buffer pointer --
	mov	ebp,_dwDelayLenG      	; get length of delay buffer
	shl	ebp,2			; mult by 4 for bytes
	add	ebp,_pdwDelayPtrG	; add start of delay buffer
	sub	ebp,4			; last position in delay buffer

MonoDelayLoop:
	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get left sample

	;-- end of delay line --
	mov	edx,edi			; copy current pointer to edx
	add	edi,4			; increment current pointer
	cmp	edi,ebp			; is past end of buffer ?
	jg	MonoDelayReset		; if so, then jump to reset

MonoDelayResetReturn:
	
	;-- feedback delayed sample --
	mov	ebx,DWORD PTR [edi]	; get feed back sample

	;-- add delayed sample to buffer --
	sar	eax,cl			; multiply by depth
	add	DWORD PTR[esi],ebx	; add to left mix buffer
	add	esi,8			; increment mix buffer pointer

	;-- add feedback sample to delay buffer --
	add	eax,ebx			; add to input sample
	xchg	ch,cl			; move coeff to cl
	sar	ebx,cl			; divide feedback sample by coeff
	xchg ch,cl			; swap back
	sub	eax,ebx			; subtract from input sample
	mov	DWORD PTR [edx],eax	; feed into delay line

	;-- decrement loop count --
	dec	dwLoopCount		; decrement loop count
	jnz	MonoDelayLoop		; if count not zero, loop

	mov	_pdwDelayPosG,edi	; write back current pointer


	POPALL

	ret

	;-- reset current pointer --
MonoDelayReset:
	mov	edi,_pdwDelayPtrG	; point to start of buffer
	jmp 	MonoDelayResetReturn

MonoDelay_ endp

;
; void StereoDelay(void)
;
StereoDelay_ proc near

	PUSHALL

	;-- setup registers --
	mov	esi,_pdwMixBufG		; pointer to mix buffer
	mov	edi,_pdwDelayPosG	; current position in delay buffer

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store 
	xor	ecx,ecx			; clear ecx
	mov	cl,_bDepthG		; depth value
	mov	ch,_bDelayCoeffG	; delay feedback coefficient 

	;-- calculate end of buffer pointer --
	mov	ebp,_dwDelayLenG      	; get length of delay buffer
	shl	ebp,2			; mult by 4 for bytes
	add	ebp,_pdwDelayPtrG	; add start of delay buffer
	sub	ebp,4			; last position in delay buffer

StereoDelayLoop:
	;-- get input samples --
	mov	eax,DWORD PTR[esi]	; get left sample
	add	eax,DWORD PTR[esi+4]	; get right sample
	sar	eax,1			; shift right	

	;-- end of delay line --
	mov	edx,edi			; copy current pointer to edx
	add	edi,4			; increment current pointer
	cmp	edi,ebp			; is past end of buffer ?
	jg	StereoDelayReset	; if so, then jump to reset

StereoDelayResetReturn:
	
	;-- feedback delayed sample --
	mov	ebx,DWORD PTR [edi]	; get feed back sample

	;-- add delayed sample to buffer --
	sar	eax,cl			; multiply by depth
	add	DWORD PTR[esi],ebx	; add to left mix buffer
	add	esi,4			; increment mix buffer pointer
	sub	DWORD PTR[esi],ebx	; sub from right mix buffer
	add	esi,4			; increment mix buffer pointer

	;-- add feedback sample to delay buffer --
	add	eax,ebx			; add to input sample
	xchg	ch,cl			; move coeff to cl
	sar	ebx,cl			; divide feedback sample by coeff
	xchg ch,cl			; swap back
	sub	eax,ebx			; subtract from input sample
	mov	DWORD PTR [edx],eax	; feed into delay line

	;-- decrement loop count --
	dec	dwLoopCount		; decrement loop count
	jnz	StereoDelayLoop		; if count not zero, loop

	mov	_pdwDelayPosG,edi	; write back current pointer
	
	POPALL

	ret

	;-- reset current pointer --
StereoDelayReset:
	mov	edi,_pdwDelayPtrG	; point to start of buffer
	jmp 	StereoDelayResetReturn

StereoDelay_ endp

;
;	void MonoReverb(void)
;					 
MonoReverb_ proc near

	PUSHALL		     

	;=== Comb filter stage ==
	;-- set up registers --
	mov	esi,_pdwMixBufG		; pointer to mix buffer
	mov	edi,_pdwWorkBufG		; pointer to work buffer

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store

	;-- calculate end of 1st buffer pointer --
	mov	ebp,_dwComb1LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb1PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd1Ptr,ebp

	;-- calculate end of 2nd buffer pointer --
	mov	ebp,_dwComb2LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb2PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd2Ptr,ebp

	;-- calculate end of 3rd buffer pointer --
	mov	ebp,_dwComb3LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb3PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd3Ptr,ebp

MonoReverbCombLoop:

	;-- get input samples --
	mov	eax,DWORD PTR [esi]		; get left sample
	add	esi,8				; increment source pointer
	mov	dwInputSample,eax		; store input sample

	;-- First Comb Filter --
	mov	ecx,_pdwComb1PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd1Ptr	    		; is past end of buffer ?
	jg	MonoReverbComb1Reset	; if so then jump to reset

MonoReverbComb1ResetReturn:
	mov	_pdwComb1PosG,ecx

	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	mov	ebp,ebx				; store sample

	add	eax,ebx				; add to input sample
	sar	ebx,2				; divide feedback sample by 4
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- Second Comb Filter --
	mov	ecx,_pdwComb2PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd2Ptr	    		; is past end of buffer ?
	jg	MonoReverbComb2Reset	; if so then jump to reset

MonoReverbComb2ResetReturn:
	mov	_pdwComb2PosG,ecx		; store position
	
	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	add	ebp,ebx				; add to Comb1 sample

	mov	eax,dwInputSample		; retrieve input sample
	add	eax,ebx				; add to input sample
	sar	ebx,3				; divide feedback sample by 8
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- Third Comb Filter --
	mov	ecx,_pdwComb3PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd3Ptr	    		; is past end of buffer ?
	jg	MonoReverbComb3Reset	; if so then jump to reset

MonoReverbComb3ResetReturn:
	mov	_pdwComb3PosG,ecx		; store position
	
	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	add	ebp,ebx				; add to Comb1 sample

	mov	eax,dwInputSample		; retrieve input sample
	add	eax,ebx				; add to input sample
	sar	ebx,3				; divide feedback sample by 8
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line


	;-- write to work buffer --
	mov	DWORD PTR [edi],ebp		; move delayed sum to work buffer
	add	edi,4				; increment work buf pointer

	;-- decrement loop count --
	dec	dwLoopCount			; decrement loop count
	jnz	MonoReverbCombLoop	; if count not zero then loop


	;=== Do final Allpass filter ==
	;-- setup registers --
	mov	esi,_pdwWorkBufG		; pointer to work buffer
	mov	edi,_pdwAllPassPosG		; pointer to delay buffer
	mov	ebx,_pdwMixBufG		; pointer to mix buffer
	xor	ecx,ecx				; clear ecx
	mov	cl,_bDepthG			; get depth scaler

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store

	;-- calculate end of buffer pointer --
	mov	ebp,_dwAllPassLenG		; get length of allpass filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwAllPassPtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwBufEnd,ebp

MonoReverbAllPassLoop:
	;-- get input sample --
	mov	eax,DWORD PTR[esi]		; get sample from work buffer
	add	esi,4				; increment work buf pointer

	;-- end of delay line --		
	mov	edx,edi				; copy  current pointer to edx
	add	edi,4				; increment current pointer
	cmp	edi,dwBufEnd		 	; is past end of buffer ?
	jg	MonoReverbAllPassReset 	; if so, then jump to reset

MonoReverbAllPassResetReturn:
	
	;-- get delayed sample --
	mov	ebp,DWORD PTR [edi]		; get feed back sample

	;-- feedback --
	add	eax,ebp				; add to input sample
	sal	eax,2				; mult input by 4
	sub	eax,ebp				; subtract from input sample
	sar	eax,2				; div input by 4
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- feedforward --	
	sub	ebp,eax				; subtract new input from feedback
	sar	eax,2				; divide by 4
	add	ebp,eax				; add new input to feedback

	;-- scale to depth --
	sar	ebp,cl				; divide by depth
	add	DWORD PTR[ebx],ebp		; add to mix buffer
	add	ebx,8				; increment pointer

	;-- decrement loop count --
	dec	dwLoopCount			; decrement loop count
	jnz	MonoReverbAllPassLoop	; if count not zero then loop

	mov	_pdwAllPassPosG,edi		; write back current pointer

	POPALL 
	ret
		
MonoReverbComb1Reset:
	mov	ecx,_pdwComb1PtrG		; point to start of buffer
	jmp	MonoReverbComb1ResetReturn

MonoReverbComb2Reset:
	mov	ecx,_pdwComb2PtrG		; point to start of buffer
	jmp	MonoReverbComb2ResetReturn

MonoReverbComb3Reset:
	mov	ecx,_pdwComb3PtrG		; point to start of buffer
	jmp	MonoReverbComb3ResetReturn

MonoReverbAllPassReset:
	mov	edi,_pdwAllPassPtrG		; point to start of buffer
	jmp	MonoReverbAllPassResetReturn

MonoReverb_ endp


;
; void StereoReverb(void)
;
StereoReverb_ proc near

	PUSHALL 

	;=== Comb filter stage ==
	;-- set up registers --
	mov	esi,_pdwMixBufG		; pointer to mix buffer
	mov	edi,_pdwWorkBufG		; pointer to work buffer

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store

	;-- calculate end of 1st buffer pointer --
	mov	ebp,_dwComb1LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb1PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd1Ptr,ebp

	;-- calculate end of 2nd buffer pointer --
	mov	ebp,_dwComb2LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb2PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd2Ptr,ebp

	;-- calculate end of 3rd buffer pointer --
	mov	ebp,_dwComb3LenG		; get length of first comb filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwComb3PtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwEnd3Ptr,ebp

StereoReverbCombLoop:

	;-- get input samples --
	mov	eax,DWORD PTR [esi]		; get left sample
	add	esi,4				; increment source pointer
	add	eax,DWORD PTR [esi]		; get right sample
	add	esi,4				; increment source pointer
	sar	eax,1				; divide by 2
	mov	dwInputSample,eax		; store input sample

	;-- First Comb Filter --
	mov	ecx,_pdwComb1PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd1Ptr	    		; is past end of buffer ?
	jg	StereoReverbComb1Reset	; if so then jump to reset

StereoReverbComb1ResetReturn:
	mov	_pdwComb1PosG,ecx

	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	mov	ebp,ebx				; store sample

	add	eax,ebx				; add to input sample
	sar	ebx,2				; divide feedback sample by 4
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- Second Comb Filter --
	mov	ecx,_pdwComb2PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd2Ptr	    		; is past end of buffer ?
	jg	StereoReverbComb2Reset	; if so then jump to reset

StereoReverbComb2ResetReturn:
	mov	_pdwComb2PosG,ecx		; store position
	
	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	add	ebp,ebx				; add to Comb1 sample

	mov	eax,dwInputSample		; retrieve input sample
	add	eax,ebx				; add to input sample
	sar	ebx,3				; divide feedback sample by 8
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- Third Comb Filter --
	mov	ecx,_pdwComb3PosG
	mov	edx,ecx 				; copy current pointer to edx
	add	ecx,4				; increment current pointer
	cmp	ecx,dwEnd3Ptr	    		; is past end of buffer ?
	jg	StereoReverbComb3Reset	; if so then jump to reset

StereoReverbComb3ResetReturn:
	mov	_pdwComb3PosG,ecx		; store position
	
	;-- get delayed sample --
	mov	ebx,DWORD PTR [ecx] 	; get feed back sample
	add	ebp,ebx				; add to Comb1 sample

	mov	eax,dwInputSample		; retrieve input sample
	add	eax,ebx				; add to input sample
	sar	ebx,3				; divide feedback sample by 8
	sub 	eax,ebx				; subtract from input sample
	mov	DWORD PTR [edx],eax		; feed into delay line


	;-- write to work buffer --
	mov	DWORD PTR [edi],ebp		; move delayed sum to work buffer
	add	edi,4				; increment work buf pointer

	;-- decrement loop count --
	dec	dwLoopCount			; decrement loop count
	jnz	StereoReverbCombLoop	; if count not zero then loop


	;=== Do final Allpass filter ==
	;-- setup registers --
	mov	esi,_pdwWorkBufG		; pointer to work buffer
	mov	edi,_pdwAllPassPosG		; pointer to delay buffer
	mov	ebx,_pdwMixBufG		; pointer to mix buffer
	xor	ecx,ecx				; clear ecx
	mov	cl,_bDepthG			; get depth scaler

	mov	eax,_dwMixBufFramesG	; size of mix buffer
	mov	dwLoopCount,eax		; store

	;-- calculate end of buffer pointer --
	mov	ebp,_dwAllPassLenG		; get length of allpass filter buffer
	shl	ebp,2				; mult by 4 for bytes
	add 	ebp,_pdwAllPassPtrG		; add start of delay buffer
	sub	ebp,4				; last position in buffer
	mov	dwBufEnd,ebp

StereoReverbAllPassLoop:
	;-- get input sample --
	mov	eax,DWORD PTR[esi]		; get sample from work buffer
	add	esi,4				; increment work buf pointer

	;-- end of delay line --		
	mov	edx,edi				; copy  current pointer to edx
	add	edi,4				; increment current pointer
	cmp	edi,dwBufEnd		 	; is past end of buffer ?
	jg	StereoReverbAllPassReset ; if so, then jump to reset

StereoReverbAllPassResetReturn:
	
	;-- get delayed sample --
	mov	ebp,DWORD PTR [edi]		; get feed back sample

	;-- feedback --
	add	eax,ebp				; add to input sample
	sal	eax,2				; mult input by 4
	sub	eax,ebp				; subtract from input sample
	sar	eax,2				; div input by 4
	mov	DWORD PTR [edx],eax		; feed into delay line

	;-- feedforward --	
	sub	ebp,eax				; subtract new input from feedback
	sar	eax,2				; divide by 4
	add	ebp,eax				; add new input to feedback

	;-- scale to depth --
	sar	ebp,cl				; divide by depth
	add	DWORD PTR[ebx],ebp		; add to mix buffer
	add	ebx,4				; increment pointer
	sub  DWORD PTR[ebx],ebp		; sub to mix buffer 
	add	ebx,4				; increment pointer

	;-- decrement loop count --
	dec	dwLoopCount			; decrement loop count
	jnz	StereoReverbAllPassLoop	; if count not zero then loop

	mov	_pdwAllPassPosG,edi		; write back current pointer

	POPALL 
	ret
		
StereoReverbComb1Reset:
	mov	ecx,_pdwComb1PtrG		; point to start of buffer
	jmp	StereoReverbComb1ResetReturn

StereoReverbComb2Reset:
	mov	ecx,_pdwComb2PtrG		; point to start of buffer
	jmp	StereoReverbComb2ResetReturn

StereoReverbComb3Reset:
	mov	ecx,_pdwComb3PtrG		; point to start of buffer
	jmp	StereoReverbComb3ResetReturn

StereoReverbAllPassReset:
	mov	edi,_pdwAllPassPtrG		; point to start of buffer
	jmp	StereoReverbAllPassResetReturn

StereoReverb_ endp

END32CODE
	
	end

