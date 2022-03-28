	include	3d.equ

	public	CoordList, NumPoints, InitClip, ClipPoints

;================================================
;	3D Clip routines - non gouraud / textured



WXL	=	0	; Window left
;WXH	=	319	; Window right
WXH	=	159	; Window right
WYL	=	0	; Window top
;WYH	=	199	; Window bottom
WYH	=	99	; Window bottom


.DATA

LastLX		dw	0
LastLY		dw	0
LastRX		dw	0
LastRY		dw	0
LastTX		dw	0
LastTY		dw	0
LastBX		dw	0
LastBY		dw	0


	; new list of co-ordinates for clipped poly
CoordList	dw	32 dup(0)
	; number of points in list
NumPoints	dd	0


;================================================

.CODE

;------------------------------------------------
; Clip Top
; IN:	AX=X, BX=Y

ClipTop		proc
		
	; if top boundary between last point and new point

		cmp	[LastTY],WYL
		jge	@@clpt1

		cmp	bx,WYL
		jge	@@doclip

	;

@@clpt1:	cmp	bx,WYL
		jge	@@noclip			; jg

		cmp	[LastTY],WYL
		jl	@@noclip			; jle

	; calculate new intersection store co-ords

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	ax,[LastTX]			; X-LastTX
		sub	bx,WYL
		neg	bx				; WYL-Y
		imul	bx				; (X-LastTX)*(WYL-Y)
		sub	si,[LastTY]			; Y-LastTY
		idiv	si				; (X-LastTX)*(WYL-Y)/(Y-LastTY)
		add	ax,cx				; (X-LastTX)*(WYH-Y)/(Y-LastTY)+X

		mov	bx,WYL
		call	AddCoords
		pop	bx ax

	; save last point

@@noclip:	mov	[LastTX],ax
		mov	[LastTY],bx

	; may still need a clip

		cmp	bx,WYL
		jl	@@exit

		call	AddCoords

@@exit:		ret
ClipTop		endp


;------------------------------------------------
; Clip bottom
; IN:	AX=X, BX=Y

ClipBottom	proc
		
	; if bottom boundary between last point and new point

		cmp	[LastBY],WYH
		jl	@@clpb1				; jle

		cmp	bx,WYH
		jl	@@doclip			; jle

	;

@@clpb1:	cmp	bx,WYH
		jl	@@noclip

		cmp	[LastBY],WYH
		jge	@@noclip

	; calculate new intersection and pass to ClipTop

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	ax,[LastBX]			; X-LastBX
		sub	bx,WYH
		neg	bx				; WYH-Y
		imul	bx				; (X-LastBX)*(WYH-Y)
		sub	si,[LastBY]			; Y-LastBY
		idiv	si				; (X-LastBX)*(WYH-Y)/(Y-LastBY)
		add	ax,cx				; (X-LastBX)*(WYH-Y)/(Y-LastBY)+X

		mov	bx,WYH
		call	ClipTop
		pop	bx ax

	; save last point

@@noclip:	mov	[LastBX],ax
		mov	[LastBY],bx

	; may still need a clip

		cmp	bx,WYH
		jge	@@exit

		call	ClipTop

@@exit:		ret
ClipBottom	endp


;------------------------------------------------
; Clip Right hand edge
; IN:	AX=X, BX=Y

ClipRight	proc
		
	; if right boundary between last point and new point

		cmp	[LastRX],WXH
		jl	@@clpr1				; jl

		cmp	ax,WXH
		jl	@@doclip			; jl

	;

@@clpr1:	cmp	ax,WXH				; X<=window right
		jl	@@noclip

		cmp	[LastRX],WXH
		jge	@@noclip

	; calculate new intersection and pass to ClipBottom

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	bx,[LastRY]			; Y-LastRY
		sub	ax,WXH
		neg	ax				; WXH-X
		imul	bx				; (Y-LastLY)*(WXL-X)
		sub	cx,[LastRX]			; X-LastLX
		idiv	cx				; (Y-LastLY)*(WXL-X)/(X-LastLX)
		add	ax,si				; (Y-LastLY)*(WXL-X)/(X-LastLX)+Y

		mov	bx,ax
		mov	ax,WXH
		call	ClipBottom
		pop	bx ax

	; save last point

@@noclip:	mov	[LastRX],ax
		mov	[LastRY],bx

	; may still need a clip

		cmp	ax,WXH
		jge	@@exit

		call	ClipBottom

@@exit:		ret
ClipRight	endp


;------------------------------------------------
; Begin clip with left hand edge
; IN:	AX=X, BX=Y

ClipPoints	proc
		
	; if left boundary between last point and new point

		cmp	[LastLX],WXL
		jge	@@clpl1

		cmp	ax,WXL
		jge	@@doclip

	;

@@clpl1:	cmp	ax,WXL				; X<=window left
		jge	@@noclip			; jg

		cmp	[LastLX],WXL
		jl	@@noclip			; jle

	; calculate new intersection and pass to ClipRight

@@doclip:	push	ax bx
		mov	cx,ax				; save X
		mov	si,bx				; save Y

		sub	bx,[LastLY]			; Y-LastLY
		sub	ax,WXL
		neg	ax				; WXL-X
		imul	bx				; (Y-LastLY)*(WXL-X)
		sub	cx,[LastLX]			; X-LastLX
		idiv	cx				; (Y-LastLY)*(WXL-X)/(X-LastLX)
		add	ax,si				; (Y-LastLY)*(WXL-X)/(X-LastLX)+Y

		mov	bx,ax
		mov	ax,WXL
		call	ClipRight
		pop	bx ax

	; save last point

@@noclip:	mov	[LastLX],ax
		mov	[LastLY],bx

	; may still need a clip

		cmp	ax,WXL
		jl	@@exit

		call	ClipRight

@@exit:		ret
ClipPoints	endp




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

InitClip	proc	near
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
InitClip	endp



		end
