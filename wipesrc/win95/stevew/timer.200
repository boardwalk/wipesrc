	include	3d.equ

;---------------------------------------------------------------------
; Vertical blank interrupt

	
	public	InstallTimerASM_, RemoveTimerASM_
	public	_TimerCount, _TimerInstalled


TimerInt	equ	1ch		; interrupt vector  of hardware timer 0
Clock		equ	1193180
@30hz		equ	39773		; Clock / 30
@50hz		equ	23863		; Clock / 50
@60hz		equ	19886		; Clock / 60
@70hz		equ	17045		; Clock / 70
@300hz		equ	3977		; Clock / 300
PIC		equ	20h		; port address - 8259a
EOI		equ	20h		; end-of-interrupt


.DATA

_TimerCount	dd	0

BiosInt		dd	0
BiosSel		dw	0

_TimerInstalled	dd	0


.CODE

	extrn	CopyBuffer_:near				; 3dpoly.asm

	extrn	UserInt_:near				; ???????.c

;---------------------------------------------------------------------
; timer interrupt
;---------------------------------------------------------------------

Timer		proc
		push	eax ebx ecx edx esi edi ebp

		sti
		cld

		mov	ax,DGROUP
		mov	ds,ax
		mov	es,ax

		inc	[_TimerCount]			; update tick counter

;		call	CopyBuffer_			; update screen If wanted

		call	UserInt_

		cli					; protect stack from xs buildup
		mov	al,EOI
		out	PIC,al				; send EOI to 8259a
		pop	ebp edi esi edx ecx ebx eax
		iretd
Timer		endp


;---------------------------------------------------------------------
; install timer interrupt
;

InstallTimerASM_	proc
		push	ds es

	; set int rate to 30hz

		mov	al,36h
		out	43h,al
		jmp	short @@l1

@@l1:		mov	al,@30hz AND 0ffh
		out	40h,al
		jmp	short @@l2

@@l2:		mov	al,@30hz/256
		out	40h,al

	; save BIOS int vector

		mov	ax,3500h+TimerInt
		int	21h
		mov	[BiosInt],ebx
		mov	[BiosSel],es

	; install new vector

		push	cs
		pop	ds
		mov	ax,2500h+TimerInt
		mov	edx,offset Timer
		int	21h

	; flag timer installed

		inc	[_TimerInstalled]

		pop	es ds
		ret
InstallTimerASM_	endp


;---------------------------------------------------------------------
; remove timer interrupt
;

RemoveTimerASM_	proc
		push	ds eax edx

		mov	al,36h				; restore timer0 for bios
		out	43h,al
		jmp	short @@l1

@@l1:		xor	al,al
		out	40h,al
		jmp	short @@l2

@@l2:		out	40h,al

		mov	ax,2500h+TimerInt
		mov	edx,[biosint]
		mov	ds,[BiosSel]
		int	21h

		dec	[_TimerInstalled]

		pop	edx eax ds
		ret
RemoveTimerASM_	endp



		end

