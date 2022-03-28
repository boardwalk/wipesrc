	include	3d.equ


	public	S_Sprite_, S_Sprite8_, S_Sprite16_, Mask_
;	public	S_Sprite_, S_Sprite8_, S_Sprite16_, Mask_, ReadJoystick_

.DATA

	extrn	LineStart:dword, _CurBuffer:dword		; 3dpoly.asm
if USE_ATI
	extrn 	_ATIpalette:word
endif
	extrn	_VRam:byte
	extrn	_DepthFadeTble:byte
	extrn	_WinPitch:dword

	extrn	_curclip:word
	extrn	_curclipS:word
if 0
	extrn	_JoyXVal:word
	extrn	_JoyYVal:word
	extrn	_JoyButtons:byte
endif


Fade    	dd	0
Transl  	dd	0

XDone		db	0
YDone		db	0
JoyResult	db	0

.CODE

	extrn	GetTPage:near

;================================================
; Draw a free size sprite
;
; In:	EDI->sprite structure

S_Sprite_	proc
		push	ebx
		mov	bx,[edi.Spr_Width]		; pick up width
		ror	ebx,16
		mov	bx,[edi.Spr_Height]		; pick up height
		call	DrawSprite
		pop	ebx
		ret
S_Sprite_	endp


;================================================
; Draw a 8*8 sprite
;
; In:	EDI->sprite structure

S_Sprite8_	proc
		push	ebx
		mov	ebx,80008h
		call	DrawSprite
		pop	ebx
		ret
S_Sprite8_	endp



;================================================
; Draw a 16*16 sprite
;
; In:	EDI->sprite structure

S_Sprite16_	proc
		push	ebx
		mov	ebx,100010h
		call	DrawSprite
		pop	ebx
		ret
S_Sprite16_	endp


;================================================
; Draw a mask
;
; IN:	EDI->sprite structure

Mask_		proc	near
		push	esi eax ebx ecx edx ebp

		mov	bx,[edi.Spr_Width]		; pick up width
		ror	ebx,16
		mov	bx,[edi.Spr_Height]		; pick up height

		mov	esi,offset _curclipS		; pick up clip data

		xor	eax,eax				; clear top of regs
		xor	edx,edx
		mov	ah,[edi.Spr_V0]			; pick up U,V co-ords
		mov	al,[edi.Spr_U0]

	; clip against top

		mov	dx,[edi.Spr_Y]			; get screen Y
		cmp	dx,[esi.WinTop]			; test if clipped
		jge	@@notc

		sub	dx,[esi.WinTop]			; test if clipped
		neg	dx
		sub	bx,dx				; modify height
		jbe	@@nodraw

		add	ah,dl				; alter V co-ord
		mov	dx,[esi.WinTop]			; new Y

	; clip against bottom

@@notc:		mov	cx,dx
		sub	cx,[esi.WinBottom]
		jae	@@nodraw

		neg	cx
		cmp	cx,bx
		jae	@@nobc

		mov	bx,cx				; new height

	; scale Y

@@nobc:		
		mov	edx,[LineStart+edx*4]
		add	edx,[_CurBuffer]

		ror	ebx,16				; get width into bx

	; clip against left

		mov	cx,[edi.Spr_X]			; pick up screen X
		cmp	cx,[esi.WinLeft]		; test if clipped
		jge	@@nolc

		sub	cx,[esi.WinLeft]		; test if clipped
		neg	cx
		sub	bx,cx				; modify width
		jbe	@@nodraw

		add	al,cl				; modify U
		mov	cx,[esi.WinLeft]		; new X

	; clip against right

@@nolc:		mov	bp,cx
		sub	bp,[esi.WinRight]
		jae	@@nodraw

		neg	bp
		cmp	bp,bx
		jae	@@done

		mov	bx,bp				; new width

	; sprite sucessfully clipped

@@done:		push	ax				; get offset into VRam
		mov	ax,[edi.Spr_tPage]
		call	GetTPage			; returns EAX = tPage<<16
		pop	ax
		mov	esi,offset _VRam
		add	esi,eax

		and	ecx,0fffh			; clear top of X
		add	edx,ecx				; EDX->buffer
if USE_ATI
		add	edx,ecx
endif
		mov	cx,bx				; pick up width & height
		ror	ebx,16

		mov	eax,0
		mov	al,[edi.Spr_r0]			; get colour
if USE_ATI
		mov	ax,[_ATIpalette+eax*2]
endif
	; draw sprite
	; ESI->sprite data
	; CX = draw width
	; EDX->buffer
	; BX = height
	; AL=colour

@@nxtline:	push	esi edx ecx

@@nxtpixel:	cmp	byte ptr [esi],0		; transparent?
		jz	@@skip

if USE_ATI
		mov	[edx],ax
else
		mov	[edx],al
endif
@@skip:		inc	esi
if USE_ATI
		add	edx,2
else
		inc	edx
endif
		dec	cx
		jnz	@@nxtpixel

		pop	ecx edx esi

	; next line
				      
		add	edx,_WinPitch		; next line in buffer
		add	esi,256				; next line in sprite
		dec	bx
		jnz	@@nxtline

@@nodraw:	pop	ebp edx ecx ebx eax esi
		ret
Mask_		endp


;================================================
; Draw sprite
; IN:	EDI->sprite structure
;	EBX=width/height (16/16 bits)

DrawSprite	proc	near

		cmp [edi.Spr_g0],0
		jnz	DrawFadeSprite			; allow the tinting of sprites
		cmp [edi.Spr_b0],1			;so b0 is greater than black not drawn
		jg	DrawTranslSprite		; allow translucent sprites SLOWER at the moment

		push	edi esi eax ebx ecx edx ebp

		mov	esi,offset _curclipS		; pick up clip data

		xor	eax,eax				; clear top of regs
		xor	edx,edx
		mov	ah,[edi.Spr_V0]			; pick up U,V co-ords
		mov	al,[edi.Spr_U0]

	; clip against top

		mov	dx,[edi.Spr_Y]			; get screen Y
		cmp	dx,[esi.WinTop]			; test if clipped
		jge	@@notc

		sub	dx,[esi.WinTop]			; test if clipped
		neg	dx
		sub	bx,dx				; modify height
		jbe	@@nodraw

		add	ah,dl				; alter V co-ord
		mov	dx,[esi.WinTop]			; new Y

	; clip against bottom

@@notc:		mov	cx,dx
		sub	cx,[esi.WinBottom]
		jae	@@nodraw

		neg	cx
		cmp	cx,bx
		jae	@@nobc

		mov	bx,cx				; new height

	; scale Y

@@nobc:		
		mov	edx,[LineStart+edx*4]
		add	edx,[_CurBuffer]

		ror	ebx,16				; get width into bx

	; clip against left

		mov	cx,[edi.Spr_X]			; pick up screen X
		cmp	cx,[esi.WinLeft]		; test if clipped
		jge	@@nolc

		sub	cx,[esi.WinLeft]		; test if clipped
		neg	cx
		sub	bx,cx				; modify width
		jbe	@@nodraw

		add	al,cl				; modify U
		mov	cx,[esi.WinLeft]		; new X

	; clip against right

@@nolc:		mov	bp,cx
		sub	bp,[esi.WinRight]
		jae	@@nodraw

		neg	bp
		cmp	bp,bx
		jae	@@done

		mov	bx,bp				; new width

	; sprite sucessfully clipped

@@done:		push	ax				; get offset into VRam
		mov	ax,[edi.Spr_tPage]
		call	GetTPage			; returns EAX = tPage<<16
		pop	ax
		mov	esi,offset _VRam
		add	esi,eax

		and	ecx,0fffh			; clear top of X
		add	edx,ecx				; EDX->buffer
if USE_ATI
		add	edx,ecx
endif

		mov	cx,bx				; pick up width & height
		ror	ebx,16

	; draw sprite
	; ESI->sprite data
	; CX = draw width
	; EDX->buffer
	; BX = height

	mov	edi,edx
	mov	edx,0

@@nxtline:	push	esi edi ecx

@@nxtpixel:	mov	dl,[esi]			; pick up pixel
		and	dl,dl				; skip transparent
		jz	@@skip

if USE_ATI
		mov	ax,[_ATIpalette+edx*2]
		mov	[edi],ax
else
		mov	[edi],dl
endif
@@skip:		inc	esi
if USE_ATI
		add	edi,2
else
		inc	edi
endif
		dec	cx
		jnz	@@nxtpixel

		pop	ecx edi esi

	; next line
				      
		add	edi,_WinPitch		; next line in buffer
		add	esi,256				; next line in sprite
		dec	bx
		jnz	@@nxtline

@@nodraw:	pop	ebp edx ecx ebx eax esi edi
		ret
DrawSprite	endp

;================================================
; Draw Fade sprite
; IN:	EDI->sprite structure
;	EBX=width/height (16/16 bits)

DrawFadeSprite	proc
		push	edi esi eax ebx ecx edx ebp

		mov	esi,offset _curclipS		; pick up clip data

		xor	eax,eax				; clear top of regs
		mov	al,[edi.Spr_g0]		; Fade Value
		dec	ax
		sal	eax,8
		mov	Fade,eax

		xor	edx,edx
		mov	ah,[edi.Spr_V0]			; pick up U,V co-ords
		mov	al,[edi.Spr_U0]

	; clip against top

		mov	dx,[edi.Spr_Y]			; get screen Y
		cmp	dx,[esi.WinTop]			; test if clipped
		jge	@@notc

		sub	dx,[esi.WinTop]			; test if clipped
		neg	dx
		sub	bx,dx				; modify height
		jbe	@@nodraw

		add	ah,dl				; alter V co-ord
		mov	dx,[esi.WinTop]			; new Y

	; clip against bottom

@@notc:		mov	cx,dx
		sub	cx,[esi.WinBottom]
		jae	@@nodraw

		neg	cx
		cmp	cx,bx
		jae	@@nobc

		mov	bx,cx				; new height

	; scale Y

@@nobc:		
		mov	edx,[LineStart+edx*4]
		add	edx,[_CurBuffer]

		ror	ebx,16				; get width into bx

	; clip against left

		mov	cx,[edi.Spr_X]			; pick up screen X
		cmp	cx,[esi.WinLeft]		; test if clipped
		jge	@@nolc

		sub	cx,[esi.WinLeft]		; test if clipped
		neg	cx
		sub	bx,cx				; modify width
		jbe	@@nodraw

		add	al,cl				; modify U
		mov	cx,[esi.WinLeft]		; new X

	; clip against right

@@nolc:		mov	bp,cx
		sub	bp,[esi.WinRight]
		jae	@@nodraw

		neg	bp
		cmp	bp,bx
		jae	@@done

		mov	bx,bp				; new width

	; sprite sucessfully clipped

@@done:		push	ax				; get offset into VRam
		mov	ax,[edi.Spr_tPage]
		call	GetTPage			; returns EAX = tPage<<16
		pop	ax
		mov	esi,offset _VRam
		add	esi,eax

		and	ecx,0fffh			; clear top of X
		add	edx,ecx				; EDX->buffer
if USE_ATI
		add	edx,ecx
endif

		mov	cx,bx				; pick up width & height
		ror	ebx,16

		mov eax,Fade			; pull back fade_index
	; draw sprite
	; ESI->sprite data
	; CX = draw width
	; EDX->buffer
	; BX = height

	mov	edi,edx
	mov	edx,0

@@nxtline:	push	esi edi ecx

@@nxtpixel:	mov	dl,[esi]			; pick up pixel
		and	dl,dl				; skip transparent
		jz	@@skip

if USE_ATI
		mov	ax,[_ATIpalette+edx*2]
		and	ax,07c00h
		mov	[edi],ax
else
		mov	dl,[edx+_DepthFadeTble]	; look up fade value 
		mov	[edi],dl
endif
@@skip:		inc	esi
if USE_ATI
		add	edi,2
else
		inc	edi
endif
		dec	cx
		jnz	@@nxtpixel

		pop	ecx edi esi

	; next line
				      
		add	edi,_WinPitch		; next line in buffer
		add	esi,256				; next line in sprite
		dec	bx
		jnz	@@nxtline

@@nodraw:	pop	ebp edx ecx ebx eax esi edi
		ret
DrawFadeSprite	endp

;================================================
; Draw Translucent sprite
; IN:	EDI->sprite structure
;	EBX=width/height (16/16 bits)

DrawTranslSprite	proc
		push	edi esi eax ebx ecx edx ebp

		mov	esi,offset _curclipS		; pick up clip data

		xor	eax,eax				; clear top of regs
		mov	al,[edi.Spr_b0]			; Fade Value
		dec	ax
		sal	eax,8
		mov	Transl,eax

		xor	edx,edx
		mov	ah,[edi.Spr_V0]			; pick up U,V co-ords
		mov	al,[edi.Spr_U0]

	; clip against top

		mov	dx,[edi.Spr_Y]			; get screen Y
		cmp	dx,[esi.WinTop]			; test if clipped
		jge	@@notc

		sub	dx,[esi.WinTop]			; test if clipped
		neg	dx
		sub	bx,dx				; modify height
		jbe	@@nodraw

		add	ah,dl				; alter V co-ord
		mov	dx,[esi.WinTop]			; new Y

	; clip against bottom

@@notc:		mov	cx,dx
		sub	cx,[esi.WinBottom]
		jae	@@nodraw

		neg	cx
		cmp	cx,bx
		jae	@@nobc

		mov	bx,cx				; new height

	; scale Y

@@nobc:		
		mov	edx,[LineStart+edx*4]
		add	edx,[_CurBuffer]

		ror	ebx,16				; get width into bx

	; clip against left

		mov	cx,[edi.Spr_X]			; pick up screen X
		cmp	cx,[esi.WinLeft]		; test if clipped
		jge	@@nolc

		sub	cx,[esi.WinLeft]		; test if clipped
		neg	cx
		sub	bx,cx				; modify width
		jbe	@@nodraw

		add	al,cl				; modify U
		mov	cx,[esi.WinLeft]		; new X

	; clip against right

@@nolc:		mov	bp,cx
		sub	bp,[esi.WinRight]
		jae	@@nodraw

		neg	bp
		cmp	bp,bx
		jae	@@done

		mov	bx,bp				; new width

	; sprite sucessfully clipped

@@done:		push	ax				; get offset into VRam
		mov	ax,[edi.Spr_tPage]
		call	GetTPage			; returns EAX = tPage<<16
		pop	ax
		mov	esi,offset _VRam
		add	esi,eax

		and	ecx,0fffh			; clear top of X
		add	edx,ecx				; EDX->buffer
if USE_ATI
		add	edx,ecx
endif

		mov	cx,bx				; pick up width & height
		ror	ebx,16

		mov eax,Transl			; pull back fade_index
	; draw sprite
	; ESI->sprite data
	; CX = draw width
	; EDX->buffer
	; BX = height

	mov	edi,edx
	mov	edx,0

@@nxtline:	push	esi edi ecx

@@nxtpixel:	mov	dl,[esi]			; pick up pixel from screen
		and	dl,dl				; skip transparent
		jz	@@skip
if USE_ATI
		mov	ax,[edi]

		and	ax,07c00h
		mov	[edi],ax
else
		mov	dl,[edi]

		mov	dl,[edx+_DepthFadeTble]	; look up fade value 
		mov	[edi],dl
endif
@@skip:		inc	esi
if USE_ATI
		add	edi,2
else
		inc	edi
endif
		dec	cx
		jnz	@@nxtpixel

		pop	ecx edi esi

	; next line
				      
		add	edi,_WinPitch		; next line in buffer
		add	esi,256				; next line in sprite
		dec	bx
		jnz	@@nxtline

@@nodraw:	pop	ebp edx ecx ebx eax esi edi
		ret
DrawTranslSprite	endp


if 0
ReadJoystick_	proc
		push	eax ebx ecx edx esi edi

		mov	bx, 0
		mov	cx, 0
		mov	al, 1

		cli          			; Disable interrupts so they don't interfere with timing
		mov 	dx, 0201h   		; Write dummy byte to joystick port
		out 	dx, al


@@joystickloop:
		in 	al, dx			; Get joystick port value
		mov	JoyResult, al
		and 	al, 1        		; Test the appropriate bit
		je	@@TestY

		inc 	bx              	; Add one to JoyXVal
		cmp 	bx, 0FFFFh       	; Check for time out (no stick)
		je 	@@done_all

@@TestY:
		mov	al, JoyResult
		and 	al, 2        		; Test the appropriate bit
		je 	@@TestDoneAll
		
		inc	cx

@@TestDoneAll:
		mov	al, JoyResult
		and 	al, 1        		; Test the appropriate bit
		jne	@@joystickloop
		mov	al, JoyResult
		and 	al, 2        		; Test the appropriate bit
		jne	@@joystickloop

@@done_all:
		mov	[_JoyXVal], bx
		mov	[_JoyYVal], cx
		in 	al, dx			; Get joystick port value
		mov	[_JoyButtons], al
		sti                    		; Enable interrupts again

		pop	edi esi edx ecx ebx eax
		ret
ReadJoystick_	endp
endif

		end
