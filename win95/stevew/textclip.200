	include	3d.equ

	public	CoordList, NumPoints
	public	TextureClip, InitTextClip
	

;================================================
;	3D Clip routines - non gouraud / textured



.DATA

	extrn	_curclip:word				; ??????.c
	extrn	_curclipS:word				; ??????.c


LastLX		dw	0
LastLY		dw	0
LastLU		dw	0
LastLV		dw	0
LastRX		dw	0
LastRY		dw	0
LastRU		dw	0
LastRV		dw	0
LastTX		dw	0
LastTY		dw	0
LastTU		dw	0
LastTV		dw	0
LastBX		dw	0
LastBY		dw	0
LastBU		dw	0
LastBV		dw	0

	; temp data storage
NewU		dw	0
NewV		dw	0

	; new list of co-ordinates for clipped poly
CoordList	dw	9*8 dup(0)

	; number of points in list
NumPoints	dd	0


;================================================

.CODE

;------------------------------------------------
; Clip Top
; IN:	AX=X, BX=Y, CX=U, DX=V
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

	; calculate X intersection store co-ords


@@doclip:	push	ax bx cx dx

		mov	NewU,cx
		mov	NewV,dx

		mov	cx,ax				; save X

	; calc frac Newdy/dy

		mov	si,bx				; SI & BX = Y
		mov	ax,[edi.WinTop]
		sub	ax,bx
		sal	eax,16				; need the precision
		mov	edx,eax				; sign extend eax to edx
		sar	edx,31
		sub	si,[LastTY]			; Y-LastTY
		sal	esi,16				; sign extend SI
		sar	esi,16
		idiv	esi				; (WYL-Y)/(Y-LastTY)
		mov	ebp,eax				; save frac

	; clip V

		mov	ax,NewV
		sub	ax,[LastTV]
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp
		shr	eax,16				; correct result
		add	ax,NewV
		push	ax				; save new V

	; clip U

		mov	ax,NewU
		sub	ax,[LastTU]
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp
		shr	eax,16				; correct result
		add	ax,NewU
		push	ax				; save new U

	; clip X

		mov	ax,cx
		sub	ax,[LastTX]			; X-LastBX
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp				; (X-LastBX)*(WYH-Y)
		shr	eax,16
		add	ax,cx				; (X-LastBX)*(WYH-Y)/(Y-LastBY)+X

	; save last point

		pop	cx
		pop	dx
		mov	bx,[edi.WinTop]
		call	AddCoords
		pop	dx cx bx ax

	; save last point

@@noclip:	mov	[LastTX],ax
		mov	[LastTY],bx
		mov	[LastTU],cx
		mov	[LastTV],dx

	; may still need a clip

		cmp	bx,[edi.WinTop]
		jl	@@exit

		call	AddCoords

@@exit:		ret
ClipTop		endp


;------------------------------------------------
; Clip bottom
; IN:	AX=X, BX=Y, CX=U, DX=V
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

	; calculate X intersection and pass to ClipTop

@@doclip:	push	ax bx cx dx

		mov	NewU,cx
		mov	NewV,dx

		mov	cx,ax				; save X

	; calc frac Newdy/dy

		mov	si,bx				; SI & BX = Y
		mov	ax,[edi.WinBottom]
		sub	ax,bx
		sal	eax,16				; need the precision
		mov	edx,eax				; sign extend eax to edx
		sar	edx,31
		sub	si,[LastBY]			; Y-LastBY
		sal	esi,16				; sign extend SI
		sar	esi,16
		idiv	esi				; (WYH-Y)/(Y-LastBY)
		mov	ebp,eax				; save frac

	; clip V

		mov	ax,NewV
		sub	ax,[LastBV]
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp
		shr	eax,16				; correct result
		add	ax,NewV
		push	ax				; save new V

	; clip U

		mov	ax,NewU
		sub	ax,[LastBU]
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp
		shr	eax,16				; correct result
		add	ax,NewU
		push	ax				; save new U

	; clip X

		mov	ax,cx
		sub	ax,[LastBX]			; X-LastBX
		sal	eax,16				; sign extend AX
		sar	eax,16
		imul	ebp				; (X-LastBX)*(WYH-Y)
		shr	eax,16
		add	ax,cx				; (X-LastBX)*(WYH-Y)/(Y-LastBY)+X

	; clip new points

		pop	cx
		pop	dx
		mov	bx,[edi.WinBottom]
		call	ClipRight

		pop	dx cx bx ax

	; save last point

@@noclip:	mov	[LastBX],ax
		mov	[LastBY],bx
		mov	[LastBU],cx
		mov	[LastBV],dx

	; may still need a clip

		cmp	bx,[edi.WinBottom]
		jge	@@exit

		call	ClipRight

@@exit:		ret
ClipBottom	endp


;------------------------------------------------
; Clip Right hand edge
; IN:	AX=X, BX=Y, CX=U, DX=V
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

	; calculate Y intersection and pass to ClipBottom

@@doclip:	push	ax bx cx dx

		mov	NewU,cx
		mov	NewV,dx

		mov	si,bx				; save Y

	; calc frac Newdx/dx

		mov	cx,ax				; AX & CX = X
		sub	ax,[edi.WinRight]
		neg	ax				; WXH-X
		sal	eax,16				; need the precision
		mov	edx,eax				; sign extend EAX into EDX
		sar	edx,31
		sub	cx,[LastRX]			; X-LastRX
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		idiv	ecx				; (WXH-X)/(X-LastRX)
		mov	ebp,eax				; save frac

	; clip V

		mov	cx,NewV
		sub	cx,[LastRV]
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		imul	ecx
		shr	eax,16
		add	ax,NewV
		push	ax				; save new V

	; clip U

		mov	eax,ebp
		mov	cx,NewU
		sub	cx,[LastRU]
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		imul	ecx
		shr	eax,16
		add	ax,NewU
		push	ax				; save new U
		
	; clip Y

		mov	eax,ebp
		sub	bx,[LastRY]			; Y-LastRY
		sal	ebx,16				; sign extend BX
		sar	ebx,16
		imul	ebx				; (Y-LastRY)*(WXH-X)
		shr	eax,16
		add	ax,si				; (Y-LastRY)*(WXH-X)/(X-LastRX)+Y

	; clip new points

		pop	cx
		pop	dx
		mov	bx,ax
		mov	ax,[edi.WinRight]
		call	ClipTop

		pop	dx cx bx ax

	; save last point

@@noclip:	mov	[LastRX],ax
		mov	[LastRY],bx
		mov	[LastRU],cx
		mov	[LastRV],dx

	; may still need a clip

		cmp	ax,[edi.WinRight]
		jge	@@exit

		call	ClipTop

@@exit:		ret
ClipRight	endp


;------------------------------------------------
; Begin clip with left hand edge
; IN:	AX=X, BX=Y, CX=U, DX=V

TextureClip	proc
		push	esi edi ebp

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

	; calculate Y intersection and pass to ClipRight

@@doclip:	push	ax bx cx dx

		mov	NewU,cx
		mov	NewV,dx

		mov	si,bx				; save Y

	; calc frac Newdx/dx

		mov	cx,ax				; CX & AX = X
		sub	ax,[edi.WinLeft]
		neg	ax				; WXL-X
		sal	eax,16				; need the precision
		mov	edx,eax				; sign extend EAX into EDX
		sar	edx,31
		sub	cx,[LastLX]			; X-LastLX
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		idiv	ecx				; EAX = fractional intersection
		mov	ebp,eax				; save frac

	; clip V

		mov	cx,NewV
		sub	cx,[LastLV]
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		imul	ecx
		shr	eax,16
		add	ax,NewV
		push	ax				; save new V

	; clip U

		mov	eax,ebp
		mov	cx,NewU
		sub	cx,[LastLU]
		sal	ecx,16				; sign extend CX
		sar	ecx,16
		imul	ecx
		shr	eax,16
		add	ax,NewU
		push	ax				; save new U
		
	; clip Y

		mov	eax,ebp
		sub	bx,[LastLY]			; Y-LastLY
		sal	ebx,16				; sign extend BX
		sar	ebx,16
		imul	ebx				; (Y-LastLY)*(WXL-X)/(X-LastLX)
		shr	eax,16
		add	ax,si				; (Y-LastLY)*(WXL-X)/(X-LastLX)+Y

	; clip new points

		pop	cx
		pop	dx
ÿ		mov	bx,ax
		mov	ax,[edi.WinLeft]
		call	ClipBottom

		pop	dx cx bx ax

	; save last point

@@noclip:	mov	[LastLX],ax
		mov	[LastLY],bx
		mov	[LastLU],cx
		mov	[LastLV],dx

	; may still need a clip

		cmp	ax,[edi.WinLeft]
		jl	@@exit

		call	ClipBottom

@@exit:		pop	ebp edi esi
		ret
TextureClip	endp




;------------------------------------------------
; add co-ordinates to list
;
; IN:	AX=X, BX=Y

AddCoords	proc	near
		mov	esi,[NumPoints]
		mov	[CoordList+esi*8],ax
		mov	[CoordList+esi*8+2],bx
		mov	[CoordList+esi*8+4],cx
		mov	[CoordList+esi*8+6],dx
		inc	[NumPoints]
		ret
AddCoords	endp


;------------------------------------------------
; Initialise the clip routines
;
; IN:	AX=StartX, BX=StartY

InitTextClip	proc	near
		mov	[LastLX],ax
		mov	[LastLY],bx
		mov	[LastLU],cx
		mov	[LastLV],dx

		mov	[LastRX],ax
		mov	[LastRY],bx
		mov	[LastRU],cx
		mov	[LastRV],dx

		mov	[LastTX],ax
		mov	[LastTY],bx
		mov	[LastTU],cx
		mov	[LastTV],dx

		mov	[LastBX],ax
		mov	[LastBY],bx
		mov	[LastBU],cx
		mov	[LastBV],dx

		mov	[NumPoints],0
		ret
InitTextClip	endp



		end
