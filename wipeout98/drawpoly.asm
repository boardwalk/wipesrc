	include	3d.equ

	public	DrawPolyF3_, DrawPolyFT3_
	public	DrawPolyF4_, DrawPolyFT4_, DrawPolyGT4_
	public	GetTPage
	public	_TextureIndex,_JJSTextureTable,_JJSTextureMenuTable

;====================================================================

.DATA
	extrn	NumPoints:dword, CoordList:word			; textclip.asm

	extrn	tPage:dword, colour:word			; 3dpoly.asm
	extrn	Transluscency:word, DepthFade:word

	extrn	_curclip:word					; ????????.c

		  
	; Table to use for calculating texture page

ifdef WIPEOUT
_TextureIndex	dd	0
_JJSTextureTable	db	00,01,02,03,04,00,01,02,03,04,05,06,07,08,09,10,16,17,18,19,20,11,20,12,13,21,14,15,16,17,18,19
_JJSTextureMenuTable	db	00,01,02,03,04,00,01,02,10,04,05,06,07,08,09,03,16,17,18,19,20,11,20,12,13,21,14,15,16,17,18,19
else

_TextureIndex	dd	2

;					00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
;TextureTable	db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,26,00,00,00,00,11,12,13,14,15,16,17,18,19,20,21,32,33,34,35,36,37,38,39,40,41,42,22,44,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 1
;				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,16,23,24,25,26,21,32,33,34,35,36,37,38,39,40,41,42,22,44,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 2
;				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,27,28,29,30,31,31,32,33,34,35,36,37,38,39,40,41,42,22,44,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Menu
;				db	00,00,00,00,00,00,00,00,00,00,00,00,00,32,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Galaxians


;					00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
;TextureTable	db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,26,00,00,00,00,11,12,13,14,15,16,17,18,19,20,21,32,33,34,35,36,37,27,28,29,30,31,22,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 1
;				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,16,23,24,25,26,21,32,33,34,35,36,37,27,28,29,30,31,22,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 2
;				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,27,28,29,30,31,31,32,33,34,35,36,37,27,28,29,30,31,22,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Menu
;				db	00,00,00,00,00,00,00,00,00,00,00,00,00,32,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Galaxians


;					00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
_JJSTextureTable	db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,26,00,00,00,00,11,20,12,13,21,14,15,16,17,18,19,32,33,34,35,36,37,27,28,29,30,31,22,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 1
				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,16,23,24,25,26,21,32,33,34,35,36,37,27,28,29,30,31,22,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Road 2
				db	00,00,00,00,00,00,01,02,03,04,05,06,07,08,09,10,00,00,00,00,00,11,12,13,14,15,17,18,19,20,21,31,32,33,34,35,36,37,27,28,29,30,31,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Menu
				db	00,00,00,00,00,00,00,00,00,00,00,00,00,32,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00		; Galaxians

endif
;====================================================================

.CODE
	extrn	DrawFlatPoly:near, DrawFlatClipped:near		; 3dpoly.asm
	extrn	DrawTextPoly:near, DrawTextClipped:near
	extrn	FlatScanConvert:near, TextScanConvert:near

	extrn	InitFlatClip:near, FlatClip:near		; flatclip.asm

	extrn	InitTextClip:near, TextureClip:near		; textclip.asm


;================================================
; Draw flat shaded poly
;
; IN:	EDI->polygon data
;
; OUT:	Polygon appears as if by magic in buffer

DrawPolyF3_	proc
		push	ebp esi

	; get colour

		mov	al,[edi.ft_r0]
		mov	ah,al
		mov	[colour],ax

	; set transluscency

		xor	ah,ah
		mov	al,[edi.ft_b0]
		mov	[Transluscency],ax

	; test if clip needed

		mov	esi,offset _curclip			; ESI->clip data

		mov	ax,[edi.ft_x0]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ft_x1]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ft_x2]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ft_y0]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ft_y1]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ft_y2]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

	; draw non clipped polygon

	; create line list for co-ords 0,1

		mov	ax,[edi.ft_x0]
		mov	dx,[edi.ft_x1]
		mov	bx,[edi.ft_y0]
		mov	si,[edi.ft_y1]
		call	FlatScanConvert

	; create line list for co-ords 0,2

		mov	ax,[edi.ft_x0]
		mov	dx,[edi.ft_x2]
		mov	bx,[edi.ft_y0]
		mov	si,[edi.ft_y2]
		call	FlatScanConvert

	; create line list for co-ords 1,2

		mov	ax,[edi.ft_x1]
		mov	dx,[edi.ft_x2]
		mov	bx,[edi.ft_y1]
		mov	si,[edi.ft_y2]
		call	FlatScanConvert

	; line list created - draw polygon into buffer

		call	DrawFlatPoly

		pop	esi ebp
		ret

	; draw clipped polygon

@@Clipit:	mov	ax,[edi.ft_x0]
		mov	bx,[edi.ft_y0]
		call	InitFlatClip

	; init pass - have to figure out a way of not doing this

		mov	ax,[edi.ft_x0]
		mov	bx,[edi.ft_y0]
		call	FlatClip
		mov	ax,[edi.ft_x1]
		mov	bx,[edi.ft_y1]
		call	FlatClip
		mov	ax,[edi.ft_x2]
		mov	bx,[edi.ft_y2]
		call	FlatClip

	; clip

		mov	[NumPoints],0
		mov	ax,[edi.ft_x0]
		mov	bx,[edi.ft_y0]
		call	FlatClip
		mov	ax,[edi.ft_x1]
		mov	bx,[edi.ft_y1]
		call	FlatClip
		mov	ax,[edi.ft_x2]
		mov	bx,[edi.ft_y2]
		call	FlatClip

	; draw clipped poly from created points list

		call	DrawFlatClipped

		pop	esi ebp
		ret
DrawPolyF3_	endp


;================================================
; Draw flat textured poly
;
; IN:	EDI->polygon data
;
; OUT:	Polygon appears as if by magic in buffer

DrawPolyFT3_	proc
		push	ebp esi

	; get colour

		jmp	@@bodge
		mov	al,[edi.ftt_r0]
		mov	ah,al
		mov	[colour],ax

		mov	ax,[edi.ftt_tpage]
		call	GetTPage

		xor	ah,ah
		mov	al,[edi.ftt_b0]
		mov	[Transluscency],ax

		mov	al,[edi.ftt_g0]
		mov	[DepthFade],ax

	; test if clip needed

		mov	esi,offset _curclip			; ESI->clip data

		mov	ax,[edi.ftt_x0]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftt_x1]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftt_x2]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftt_y0]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ftt_y1]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ftt_y2]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

	; draw non clipped polygon

	; create line list for co-ords 0,1
	; This is my version for TextScanConvert AJY
		mov	ax,[edi.ftt_x1]
		sal	eax,16
		mov	ax,[edi.ftt_x0]
		mov	dx,[edi.ftt_y1]
		sal edx,16
		mov	dx,[edi.ftt_y0]
		mov	ch,[edi.ftt_u0]
		mov	cl,[edi.ftt_u1]
		mov	bh,[edi.ftt_v0]
		mov	bl,[edi.ftt_v1]

		call	TextScanConvert

	; create line list for co-ords 0,2

	; This is my version for TextScanConvert AJY
		mov	ax,[edi.ftt_x2]
		sal	eax,16
		mov	ax,[edi.ftt_x0]
		mov	dx,[edi.ftt_y2]
		sal	edx,16
		mov	dx,[edi.ftt_y0]
		mov	ch,[edi.ftt_u0]
		mov	cl,[edi.ftt_u2]
		mov	bh,[edi.ftt_v0]
		mov	bl,[edi.ftt_v2]

		call	TextScanConvert

	; create line list for co-ords 1,2
	; This is my version for TextScanConvert AJY

		mov	ax,[edi.ftt_x2]
		sal	eax,16
		mov	ax,[edi.ftt_x1]
		mov	dx,[edi.ftt_y2]
		sal	edx,16
		mov	dx,[edi.ftt_y1]
		mov	ch,[edi.ftt_u1]
		mov	cl,[edi.ftt_u2]
		mov	bh,[edi.ftt_v1]
		mov	bl,[edi.ftt_v2]

		call	TextScanConvert

	; line list created - draw polygon into buffer

		call	DrawTextPoly

		pop	esi ebp
		ret

	; draw clipped polygon

@@Clipit:	mov	ax,[edi.ftt_x0]
		mov	bx,[edi.ftt_y0]
		call	InitTextClip

	; init pass - have to figure out a way of not doing this

		mov	ax,[edi.ftt_x0]
		mov	bx,[edi.ftt_y0]
		call	TextureClip
		mov	ax,[edi.ftt_x1]
		mov	bx,[edi.ftt_y1]
		call	TextureClip
		mov	ax,[edi.ftt_x2]
		mov	bx,[edi.ftt_y2]
		call	TextureClip

	; clip

		mov	[NumPoints],0
		mov	ax,[edi.ftt_x0]
		mov	bx,[edi.ftt_y0]
		call	TextureClip
		mov	ax,[edi.ftt_x1]
		mov	bx,[edi.ftt_y1]
		call	TextureClip
		mov	ax,[edi.ftt_x2]
		mov	bx,[edi.ftt_y2]
		call	TextureClip

	; draw clipped poly from created points list
@@bodge:
		call	DrawTextClipped
		
		pop	esi ebp
		ret
DrawPolyFT3_	endp


;================================================
; Draw flat shaded quad
;
; IN:	EDI->Quad data
;
; OUT:	Quad appears as if by magic in buffer

DrawPolyF4_	proc
		push	ebp esi

	; get colour

		mov	al,[edi.fq_r0]
		mov	ah,al
		mov	[colour],ax

		xor	ah,ah
		mov	al,[edi.fq_b0]
		mov	[Transluscency],ax

	; test if clip needed

		mov	esi,offset _curclip			; ESI->clip data

		mov	ax,[edi.fq_x0]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.fq_x1]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.fq_x2]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.fq_x3]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.fq_y0]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.fq_y1]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.fq_y2]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.fq_y3]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

	; draw non clipped polygon

	; create line list for co-ords 0,1

		mov	ax,[edi.fq_x0]
		mov	dx,[edi.fq_x1]
		mov	bx,[edi.fq_y0]
		mov	si,[edi.fq_y1]
		call	FlatScanConvert

	; create line list for co-ords 1,3

		mov	ax,[edi.fq_x1]
		mov	dx,[edi.fq_x3]
		mov	bx,[edi.fq_y1]
		mov	si,[edi.fq_y3]
		call	FlatScanConvert

	; create line list for co-ords 3,2

		mov	ax,[edi.fq_x3]
		mov	dx,[edi.fq_x2]
		mov	bx,[edi.fq_y3]
		mov	si,[edi.fq_y2]
		call	FlatScanConvert

	; create line list for co-ords 2,0

		mov	ax,[edi.fq_x2]
		mov	dx,[edi.fq_x0]
		mov	bx,[edi.fq_y2]
		mov	si,[edi.fq_y0]
		call	FlatScanConvert

	; line list created - draw polygon into buffer

		call	DrawFlatPoly

		pop	esi ebp
		ret

	; draw clipped polygon

@@Clipit:	mov	ax,[edi.fq_x0]
		mov	bx,[edi.fq_y0]
		call	InitFlatClip

	; init pass - have to figure out a way of not doing this

		mov	ax,[edi.fq_x0]
		mov	bx,[edi.fq_y0]
		call	FlatClip
		mov	ax,[edi.fq_x1]
		mov	bx,[edi.fq_y1]
		call	FlatClip
		mov	ax,[edi.fq_x3]
		mov	bx,[edi.fq_y3]
		call	FlatClip
		mov	ax,[edi.fq_x2]
		mov	bx,[edi.fq_y2]
		call	FlatClip

	; clip

		mov	[NumPoints],0
		mov	ax,[edi.fq_x0]
		mov	bx,[edi.fq_y0]
		call	FlatClip
		mov	ax,[edi.fq_x1]
		mov	bx,[edi.fq_y1]
		call	FlatClip
		mov	ax,[edi.fq_x3]
		mov	bx,[edi.fq_y3]
		call	FlatClip
		mov	ax,[edi.fq_x2]
		mov	bx,[edi.fq_y2]
		call	FlatClip

	; draw clipped poly from created points list

		call	DrawFlatClipped

		pop	esi ebp
		ret
DrawPolyF4_	endp


;================================================
; Draw goraud textured quad
;
; IN:	EDI->Quad data
;
; OUT:	Quad appears as if by magic in buffer

DrawPolyGT4_	proc
		ret
DrawPolyGT4_	endp


;================================================
; Draw flat shaded quad
;
; IN:	EDI->Quad data
;
; OUT:	Quad appears as if by magic in buffer

DrawPolyFT4_	proc
		push	ebp esi

	; get colour

		mov	al,[edi.ftq_r0]
		mov	ah,al
		mov	[colour],ax

		mov	ax,[edi.ftq_tpage]
		call	GetTPage

		xor	ah,ah
		mov	al,[edi.ftq_b0]
		mov	[Transluscency],ax

		mov	al,[edi.ftq_g0]
		mov	[DepthFade],ax

	; test if clip needed

		mov	esi,offset _curclip			; ESI->clip data

		mov	ax,[edi.ftq_x0]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftq_x1]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftq_x2]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftq_x3]
		sub	ax,[esi.WinLeft]
		cmp	ax,[esi.WinWidth]
		jnc	@@Clipit

		mov	ax,[edi.ftq_y0]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ftq_y1]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ftq_y2]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

		mov	ax,[edi.ftq_y3]
		sub	ax,[esi.WinTop]
		cmp	ax,[esi.WinHeight]
		jnc	@@Clipit

	; draw non clipped polygon

	; create line list for co-ords 0,1

		mov	ax,[edi.ftq_x1]
		sal	eax,16
		mov	ax,[edi.ftq_x0]
		mov	dx,[edi.ftq_y1]
		sal	edx,16
		mov	dx,[edi.ftq_y0]
		mov	ch,[edi.ftq_u0]
		mov	cl,[edi.ftq_u1]
		mov	bh,[edi.ftq_v0]
		mov	bl,[edi.ftq_v1]
		call	TextScanConvert

	; create line list for co-ords 1,3

		mov	ax,[edi.ftq_x3]
		sal	eax,16
		mov	ax,[edi.ftq_x1]
		mov	dx,[edi.ftq_y3]
		sal	edx,16
		mov	dx,[edi.ftq_y1]
		mov	ch,[edi.ftq_u1]
		mov	cl,[edi.ftq_u3]
		mov	bh,[edi.ftq_v1]
		mov	bl,[edi.ftq_v3]
		call	TextScanConvert

	; create line list for co-ords 3,2

		mov	ax,[edi.ftq_x2]
		sal	eax,16
		mov	ax,[edi.ftq_x3]
		mov	dx,[edi.ftq_y2]
		sal	edx,16
		mov	dx,[edi.ftq_y3]
		mov	ch,[edi.ftq_u3]
		mov	cl,[edi.ftq_u2]
		mov	bh,[edi.ftq_v3]
		mov	bl,[edi.ftq_v2]
		call	TextScanConvert

	; create line list for co-ords 2,0

		mov	ax,[edi.ftq_x0]
		sal	eax,16
		mov	ax,[edi.ftq_x2]
		mov	dx,[edi.ftq_y0]
		sal	edx,16
		mov	dx,[edi.ftq_y2]
		mov	ch,[edi.ftq_u2]
		mov	cl,[edi.ftq_u0]
		mov	bh,[edi.ftq_v2]
		mov	bl,[edi.ftq_v0]
		call	TextScanConvert

	; line list created - draw polygon into buffer

		call	DrawTextPoly

		pop	esi ebp
		ret

	; draw clipped polygon

@@Clipit:	mov	ax,[edi.ftq_x0]
		mov	bx,[edi.ftq_y0]
		call	InitTextClip

	; init pass - have to figure out a way of not doing this

		mov	ax,[edi.ftq_x0]
		mov	bx,[edi.ftq_y0]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u0]
		mov	dl,[edi.ftq_v0]
		call	TextureClip

		mov	ax,[edi.ftq_x1]
		mov	bx,[edi.ftq_y1]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u1]
		mov	dl,[edi.ftq_v1]
		call	TextureClip

		mov	ax,[edi.ftq_x3]
		mov	bx,[edi.ftq_y3]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u3]
		mov	dl,[edi.ftq_v3]
		call	TextureClip

		mov	ax,[edi.ftq_x2]
		mov	bx,[edi.ftq_y2]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u2]
		mov	dl,[edi.ftq_v2]
		call	TextureClip
	; clip

		mov	[NumPoints],0
		mov	ax,[edi.ftq_x0]
		mov	bx,[edi.ftq_y0]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u0]
		mov	dl,[edi.ftq_v0]
		call	TextureClip

		mov	ax,[edi.ftq_x1]
		mov	bx,[edi.ftq_y1]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u1]
		mov	dl,[edi.ftq_v1]
		call	TextureClip

		mov	ax,[edi.ftq_x3]
		mov	bx,[edi.ftq_y3]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u3]
		mov	dl,[edi.ftq_v3]
		call	TextureClip

		mov	ax,[edi.ftq_x2]
		mov	bx,[edi.ftq_y2]
		xor	ch,ch
		xor	dh,dh
		mov	cl,[edi.ftq_u2]
		mov	dl,[edi.ftq_v2]
		call	TextureClip

	; draw clipped poly from created points list

		mov	edx,8
		call	CheckPoints
		jnz	@@skip

		call	DrawTextClipped

@@skip:		pop	esi ebp
		ret
DrawPolyFT4_	endp


;================================================
; Get correct texture page based on index
;
; IN: AX=tPage to convert
;
; OUT: EAX=new tPage << 16

GetTPage	proc
		push	ebx

ifdef WIPEOUT
		and	eax,1fh			;Changed to allow more textures
else
		and	eax,3fh			;Changed to allow more textures
endif
		mov	ebx,[_TextureIndex]
		and	ebx,3
		sal	ebx,6			; Changed for more textures AJY
		mov	al,[ebx+eax+_JJSTextureTable]

		sal	eax,16
		mov	[tPage],eax

		pop	ebx
		ret
GetTPage	endp



CheckPoints	proc
		push	edi
		mov	ecx,[NumPoints]
		test	cx,cx
		jz	@@ok

		mov	esi,offset CoordList
		mov	edi,offset _curclip

@@l1:		mov	ax,[esi]
		mov	bx,[esi+2]

		sub	ax,[edi.WinLeft]
		cmp	ax,[edi.WinWidth]
		jnbe	@@err

		sub	bx,[edi.WinTop]
		cmp	bx,[edi.WinHeight]
		jnbe	@@err

		add	esi,edx
		dec	cx
		jnz	@@l1

@@ok:		xor	ax,ax
		test	ax,ax
		pop	edi
		ret

@@err:		mov	ax,-1
		test	ax,ax
		pop	edi
		ret
CheckPoints	endp


		end

