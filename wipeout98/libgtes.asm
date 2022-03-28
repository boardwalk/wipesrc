		SHIFT 	equ 	12

		public 	ApplyMatrix_
		public	DLSRotTransPers_
		public 	AverageZ3_
		public 	AverageZ4_

.386P

dgroup	group _data


		





_data		segment para public use32 'data'
		assume ds:_data

; Declare any variables that belong here but are shared else where as ... PUBLIC _Variable
; Remember to declare the variables in the C code as ... EXTERN Variable

; Declare any variables that belong in the C code but are shared here as ... EXTRN _Variable:Size  Size = WORD, DWORD etc
; Nothing special is needed in the C code to export the variable to here except that the leading underscore is not used

xt		dd	?	; long value
yt		dd	?	; long value
zt		dd	?	; long value

sxy		dd	?
r0		dd	?
flag		dd	?
flagaddr	dd	?

x		dd	?
y		dd	?
z		dd	?	; long value

tmpDW1		dd	?	; temporary storage

parm1		dd	?	; temporary parmeter store
parm2		dd	?	; temporary parmeter store
parm3		dd	?	; temporary parmeter store

RotMatrix struc
	m	dw 9 dup(0)
	pad	dw 0
	t	dd 3 dup(0)
RotMatrix ends

EXTRN _xOffset:DWORD
EXTRN _yOffset:DWORD
EXTRN _screenZ:DWORD
EXTRN _SCR2:DWORD
EXTRN _FNEAR:DWORD
EXTRN _FFAR:DWORD
EXTRN _MaxZValue:DWORD
EXTRN _HalfZValue:DWORD

_data		ends








_text		segment para public use32 'code'
		assume	cs:_text



;------------------------------------------------------
; ApplyMatrix
;
;	Multiplies RTM by v0 and puts the
;	result into r0 (or whatever)
;
;	Inputs:
; 		eax = RTM
;		ebx = v0 (input vector)
;		ecx = r0 (output)
;------------------------------------------------------

ApplyMatrix_:

		push 	eax ebx ecx edx edi

		mov 	edi, eax		; store parameters
		mov 	edx, ecx

		mov	cx, [ebx]
		mov	ax, [ebx+2]
		mov	bx, [ebx+4]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		mov 	xt, ecx
		mov 	yt, eax
		mov 	zt, ebx


		mov  	cx, [edi]		
		mov	ax, [edi+2]
		mov	bx, [edi+4]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
;		sar	ecx, 17		; DLS
;		sar	eax, 17
;		sar	ebx, 17
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
;		sar	ecx, SHIFT	; -1
		add	ecx, [edi+20]
		mov 	+0[edx], ecx		; store value in vector->vx


		mov  	cx, [edi+6]		
		mov	ax, [edi+8]
		mov	bx, [edi+10]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
;		sar	ecx, 17
;		sar	eax, 17
;		sar	ebx, 17
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
;		sar	ecx, SHIFT	; -1
		add	ecx, [edi+24]
		mov 	+4[edx], ecx		; store value in vector->vx

		mov  	cx, [edi+12]		
		mov	ax, [edi+14]
		mov	bx, [edi+16]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
;		sar	ecx, 17
;		sar	eax, 17
;		sar	ebx, 17
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
;		sar	ecx, SHIFT	; -1
		add	ecx, [edi+28]
		mov 	+8[edx], ecx		; store value in vector->vx

		pop 	edi edx ecx ebx eax
		ret

;------------------------------------------------------
; DLSRotTransPers
;
;	Moves the body of RotTransPers into
;	an assembly function
;
;	Inputs:
;		eax = address of screen coords
;		ebx = address of input coords
;		ecx = address of flag variable
;------------------------------------------------------

DLSRotTransPers_:
		push 	eax ebx ecx edx

		mov	sxy, eax
		mov	r0, ebx
		mov	flagaddr, ecx
		mov	[flag], 0			; clear flag

		mov	eax, [ebx]		; move r0.vx into x
		mov	x, eax
		mov	eax, [ebx+4]		; move r0.vy into y
		mov	y, eax
		mov	eax, [ebx+8]		; move r0.vz into eax
		mov	z, eax

		cmp	z, 0
		jge	@@nonegz
		or	[flag], 00040000h

@@nonegz:
		mov	ebx, [_HalfZValue]
		cmp	z, ebx
		jle	@@nobigz
		or	[flag], 00400000h

@@nobigz:
		mov	ebx, [_SCR2]
		cmp	z, ebx
		jge	@@noscr2clip
		mov	z, ebx
		or	[flag], 00020000h

@@noscr2clip:
		mov	ebx, [_MaxZValue]
		cmp	z, ebx
		jle	@@nomaxzclip
		mov	z, ebx

@@nomaxzclip:
		mov	ebx, [_FNEAR]
		cmp	z, ebx
		jge	@@nozfnear
		or	[flag], 00001000h
		jmp	@@nozffar
		
@@nozfnear:
		mov	ebx, [_FFAR]
		cmp	z, ebx
		jle	@@nozffar
		or	[flag], 00001000h
		
@@nozffar:
		cmp	x, 32767
		jle	@@nomaxxclip
		mov	x, 32767
		or	[flag], 81000000h

@@nomaxxclip:
		cmp	y, 32767
		jle	@@nomaxyclip
		mov	y, 32767
		or	[flag], 80800000h

@@nomaxyclip:
		cmp	x, -32767
		jge	@@nominxclip
		mov	x, -32767
		or	[flag], 81000000h

@@nominxclip:
		cmp	y, -32767
		jge	@@nominyclip
		mov	y, -32767
		or	[flag], 80800000h

@@nominyclip:
		mov	eax,[_screenZ]		; Finished checks, do mul
		imul	x
		idiv	z
		add	eax,[_xOffset]
		mov	ebx, eax		; xt0 in ebx

		mov	eax,[_screenZ]		; Finished checks, do mul
		imul	y
		idiv	z
		add	eax,[_yOffset]
		mov	ecx, eax		; yt0 in ecx

		cmp	ebx, -1024		; clip the screen coords
		jge	@@nominxt0clip
		mov	ebx, -1024
		or	[flag], 80004000h

@@nominxt0clip:
		cmp	ebx, 1024
		jle	@@nomaxxt0clip
		mov	ebx, 1024
		or	[flag], 80004000h

@@nomaxxt0clip:
		cmp	ecx, -1024 
		jge	@@nominyt0clip
		mov	ecx, -1024
		or	[flag], 80002000h

@@nominyt0clip:
		cmp	ecx, 1024
		jle	@@nomaxyt0clip
		mov	ecx, 1024
		or	[flag], 80002000h

@@nomaxyt0clip:
		and	ebx, 0000FFFFh		
		sal	ecx, 16			; equivalent to
		or	ebx, ecx		; (yt0 << 16) | (xt0 & 0x0000FFFF);
		mov	edx, sxy
		mov	[edx], ebx		; result stored in sxy
		mov	ebx, [flag]
		mov	edx, flagaddr
		mov	[edx], ebx		; flag result stored in flag
		
		pop 	edx ecx ebx eax
		ret


;
; AverageZ3
;

AverageZ3_:
		clc
		adc eax, ebx
		adc eax, ecx

		sar eax, 2		; perform divide using shifts and additions
		mov ebx, eax
		sar ebx, 2		; 1/16
		adc eax, ebx
		sar ebx, 2		; 1/64
		adc eax, ebx
		sar ebx, 2		; 1/256
		adc eax, ebx
		sar ebx, 2		; 1/1024
		adc eax, ebx
		sar eax, 2		; 1/4 of the answer
		ret


AverageZ4_:
		clc
		adc eax, ebx
		adc eax, ecx

		sar eax, 4
		ret



_text		ends
 		end

