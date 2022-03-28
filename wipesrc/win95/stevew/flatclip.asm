	include	3d.equ

	public	InitFlatClip, FlatClip

;================================================
;	3D Clip routines - non gouraud / textured


.DATA

	extrn	_curclip:word					; ??????.c

	extrn	_curclipS:word					; ??????.c

	extrn	CoordList:word, NumPoints:dword			; TextClip.asm

LastLX		dw	0
LastLY		dw	0
LastRX		dw	0
LastRY		dw	0
LastTX		dw	0
LastTY		dw	0
LastBX		dw	0
LastBY		dw	0


;================================================

.CODE

;------------------------------------------------
; Clip Top
; IN:	AX=X, BX=Y
;	EDI->_curclip

ClipTop		proc
		
	; if top boundary between last point and new point

		mov	si,[LastTY]

		cmp	si,[edi.WinTop]
		jge	@@clpt1

		cmp	bx,[edi.WinTop]
		jge	@@doclip

	;

@@clpt1:	cmp	bx,[edi.WinTop]
		jge	@@noclip

		cmp	si,[edi.WinTop]
		jl	@@noclip

	; calculate new intersection store co-ords

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	ax,[LastTX]			; X-LastTX
		sub	bx,[edi.WinTop]
		neg	bx				; WYL-Y
		imul	bx				; (X-LastTX)*(WYL-Y)
		sub	si,[LastTY]			; Y-LastTY
		idiv	si				; (X-LastTX)*(WYL-Y)/(Y-LastTY)
		add	ax,cx				; (X-LastTX)*(WYH-Y)/(Y-LastTY)+X

		mov	bx,[edi.WinTop]
		call	AddCoords
		pop	bx ax

	; save last point

@@noclip:	mov	[LastTX],ax
		mov	[LastTY],bx

	; may still need a clip

		cmp	bx,[edi.WinTop]
		jl	@@exit

		call	AddCoords

@@exit:		ret
ClipTop		endp


;------------------------------------------------
; Clip bottom
; IN:	AX=X, BX=Y
;	EDI->_curclip

ClipBottom	proc
		
	; if bottom boundary between last point and new point

		mov	si,[LastBY]

		cmp	si,[edi.WinBottom]
		jl	@@clpb1

		cmp	bx,[edi.WinBottom]
		jl	@@doclip

	;

@@clpb1:	cmp	bx,[edi.WinBottom]
		jl	@@noclip

		cmp	si,[edi.WinBottom]
		jge	@@noclip

	; calculate new intersection and pass to ClipTop

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	ax,[LastBX]			; X-LastBX
		sub	bx,[edi.WinBottom]
		neg	bx				; WYH-Y
		imul	bx				; (X-LastBX)*(WYH-Y)
		sub	si,[LastBY]			; Y-LastBY
		idiv	si				; (X-LastBX)*(WYH-Y)/(Y-LastBY)
		add	ax,cx				; (X-LastBX)*(WYH-Y)/(Y-LastBY)+X

		mov	bx,[edi.WinBottom]
		call	ClipTop
		pop	bx ax

	; save last point

@@noclip:	mov	[LastBX],ax
		mov	[LastBY],bx

	; may still need a clip

		cmp	bx,[edi.WinBottom]
		jge	@@exit

		call	ClipTop

@@exit:		ret
ClipBottom	endp


;------------------------------------------------
; Clip Right hand edge
; IN:	AX=X, BX=Y
;	EDI->_curclip

ClipRight	proc
		
	; if right boundary between last point and new point

		mov	si,[LastRX]

		cmp	si,[edi.WinRight]
		jl	@@clpr1

		cmp	ax,[edi.WinRight]
		jl	@@doclip

	;

@@clpr1:	cmp	ax,[edi.WinRight]		; X<=window right
		jl	@@noclip

		cmp	si,[edi.WinRight]
		jge	@@noclip

	; calculate new intersection and pass to ClipBottom

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	bx,[LastRY]			; Y-LastRY
		sub	ax,[edi.WinRight]
		neg	ax				; WXH-X
		imul	bx				; (Y-LastLY)*(WXL-X)
		sub	cx,[LastRX]			; X-LastLX
		idiv	cx				; (Y-LastLY)*(WXL-X)/(X-LastLX)
		add	ax,si				; (Y-LastLY)*(WXL-X)/(X-LastLX)+Y

		mov	bx,ax
		mov	ax,[edi.WinRight]
		call	ClipBottom
		pop	bx ax

	; save last point

@@noclip:	mov	[LastRX],ax
		mov	[LastRY],bx

	; may still need a clip

		cmp	ax,[edi.WinRight]
		jge	@@exit

		call	ClipBottom

@@exit:		ret
ClipRight	endp


;------------------------------------------------
; Begin clip with left hand edge
; IN:	AX=X, BX=Y
;	EDI->_curclip

FlatClip	proc
		push	esi edi
		
		mov	edi,offset _curclip

	; if left boundary between last point and new point

		mov	si,[LastLX]

		cmp	si,[edi.WinLeft]
		jge	@@clpl1

		cmp	ax,[edi.WinLeft]
		jge	@@doclip

	;

@@clpl1:	cmp	ax,[edi.WinLeft]		; X<=window left
		jge	@@noclip

		cmp	si,[edi.WinLeft]
		jl	@@noclip

	; calculate new intersection and pass to ClipRight

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	bx,[LastLY]			; Y-LastLY
		sub	ax,[edi.WinLeft]
		neg	ax				; WXL-X
		imul	bx				; (Y-LastLY)*(WXL-X)
		sub	cx,[LastLX]			; X-LastLX
		idiv	cx				; (Y-LastLY)*(WXL-X)/(X-LastLX)
		add	ax,si				; (Y-LastLY)*(WXL-X)/(X-LastLX)+Y

		mov	bx,ax
		mov	ax,[edi.WinLeft]
		call	ClipRight
		pop	bx ax

	; save last point

@@noclip:	mov	[LastLX],ax
		mov	[LastLY],bx

	; may still need a clip

		cmp	ax,[edi.WinLeft]
		jl	@@exit

		call	ClipRight

@@exit:		pop	edi esi
		ret
FlatClip	endp




;------------------------------------------------
; add co-ordinates to list
;
; IN:	AX=X, BX=Y

AddCoords	proc	near
		mov	edx,[NumPoints]
		mov	[CoordList+edx*4],ax
		mov	[CoordList+edx*4+2],bx
		inc	[NumPoints]
		ret
AddCoords	endp


;------------------------------------------------
; Initialise the clip routines
;
; IN:	AX=StartX, BX=StartY

InitFlatClip	proc	near
		mov	[LastLX],ax
		mov	[LastLY],bx
		mov	[LastRX],ax
		mov	[LastRY],bx
		mov	[LastTX],ax
		mov	[LastTY],bx
		mov	[LastBX],ax
		mov	[LastBY],bx
		mov	[NumPoints],0
		ret
InitFlatClip	endp



		end
