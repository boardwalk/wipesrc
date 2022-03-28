	include	3d.equ

	public	DrawFlatClipped, DrawFlatPoly
	public	DrawTextClipped, DrawTextPoly
	public	FlatScanConvert, TextScanConvert, tPage
	public	SetVMode_, ResetVMode_, GSwap_, WaitVbl_, Pen_
	public	ClearBuf_, ClearGameBuf_, colour, InitFpTbl_, _VRam
	public	_JRam 					;jed
;	Randy_, 
	public	LineStart, _CurBuffer, ResetPitch_
	public	_CurBufferJ				;jed
	public	Transluscency, CopyBuffer_,stuffem256_
	public  DepthFade, _DepthFadeTble

;	public	_curclip

;====================================================================

.DATA
	extrn	CoordList:word, NumPoints:dword			; textclip.asm
	extrn	_WinPitch:dword, _WinHeight:word, _WinWidth:word
	extrn	_WinHeightX2:word


_CurBuffer	label	dword
		dd	0		;filled in later by Windows now.
		dd 	0

_CurBufferJ	label	dword	;jed
		dd	0

MinFillLine	dw	32767

CopyFlag	dw	0

tPage		dd	0

Transluscency	dw	0
DepthFade	dw	0

MinTbl		dw	buffer_depth dup(32767)
MaxTbl		dw	buffer_depth dup(-32767)
UMinTbl		db	buffer_depth dup(?)
UMaxTbl		db	buffer_depth dup(?)
VMinTbl		db	buffer_depth dup(?)
VMaxTbl		db	buffer_depth dup(?)


LineStart	label	dword
	i = 0
		rept	480
		dd	i
	i = i+320
		endm

ReDrawScreen	dw	-1

Colour		dw	00f0fh

RandNum		dd	1



	; table for depth fading to black

_DepthFadeTble	db	256*96 dup(0)


	; divide table, format AL/AH (8 bit by 9 bit)
DivTable	dd	640*256 dup(?)

	; VRAM
_VRam		label	byte
ifdef WIPEOUT
	;	db	256*256*96 dup(0) ; 96 tpages for PCwipeout 24 bit dump mode
		db	256*256*22 dup(0)
else
;		db	256*256*43 dup(?)
		db	256*256*38 dup(?)
endif

_JRam		db	320*200 dup(0); My Ram Buffer	;jed


;_curclip	ClipStruc<0,0,319,239,319,239>

;====================================================================


.CODE

ResetPitch_ proc
			push 	eax
			push	ecx
			push	esi

;			Reset the Line lookup table
			mov		ecx,480
			xor		eax,eax
			mov		esi, offset LineStart
@@loop:		mov		[esi],eax
			add		eax,_WinPitch
			add		esi, 4		;dd
			dec		ecx
			jnz		@@loop


			pop		esi
			pop		ecx
			pop  	eax
			ret
ResetPitch_ endp

SetVMode_	proc
		mov	ax,13h
		int	10h
		ret
SetVMode_	endp

ResetVMode_	proc
		mov	ax,02h
		int	10h
		ret
ResetVMode_	endp


;================================================
; Initialise tables

InitFpTbl_	proc

	;----------------------------------------
	; init fixed point division table

		push	eax ebx ecx edx esi

		xor	eax,eax
		xor	ebx,ebx
		mov	esi,offset DivTable

@@l1:		push	eax
		sal	eax,16
		xor	edx,edx
		test	ebx,ebx
		jnz	@@l2

		mov	eax,0
		jmp	short @@l3

@@l2:		div	ebx

@@l3:		mov	[esi],eax
		add	esi,4
		pop	eax

		inc	al
		jnz	@@l1

		inc	bx
		cmp	bx,640
		jnz	@@l1

	;----------------------------------------
	; init fade to black table
		if	0		
		mov	ebx,offset _DepthFadeTble
		xor	dx,dx				; start with colour 0

	; just store first colour

@@fd2:		ror	dx,1				; drop precision for blue
		mov	[ebx],dl			; store colour
		rol	dx,1
		
	; fade a single colour, though 7 levels

		mov	cl,7
		push	ebx edx

		sal	dx,1				; add extra bit for blue
@@fd1:		add	ebx,256				; drop through levels of fading
		call	@@fade
		dec	cl
		jnz	@@fd1

	; next colour

		pop	edx ebx
		inc	ebx				; ->+1

		inc	dl				; next colour
		jnz	@@fd2
		endif

		pop	esi edx ecx ebx eax
		ret


	; fade colours
	; IN:	DX=colour RGB 3 bits per colour
	;	EBX=address to store faded colour

@@fade:
		ror	dx,1				; drop precision for blue
		mov	[ebx],dl			; store colour
		rol	dx,1

		ret

		mov	ax,dx
		and	ax,111000000b			; mask out red
		sub	ax,001000000b			; fade red
		jc	@@f1

		and	dx,000111111b
		or	dx,ax

@@f1:		mov	ax,dx
		and	ax,000111000b			; mask out green
		sub	ax,000001000b			; fade green
		jc	@@f2

		and	dx,111000111b
		or	dx,ax

@@f2:		mov	ax,dx
		and	ax,000000111b			; mask out blue
		sub	ax,000000001b			; fade blue
		jc	@@f3

		and	dx,111111000b
		or	dx,ax

	; store colour

@@f3:		ror	dx,1				; drop precision for blue
		mov	[ebx],dl			; store colour
		rol	dx,1

		ret
InitFpTbl_	endp


;================================================
; Draw clipped flat polygon - non textured - non gouraud
;

DrawFlatClipped	proc
		mov	ecx,[NumPoints]			; number of pairs of coords
		test	cx,cx
		jz	@@nodraw

		mov	esi,offset CoordList		; get ptr to clipped coords

	; insert first coordinates for endpoints of last line

		mov	ax,[esi]			; get 1st coords
		mov	bx,[esi+2]
		mov	[esi+ecx*4],ax			; save at end of list
		mov	[esi+ecx*4+2],bx

	; create scan list

@@lp1:		push	esi ecx
		mov	ax,[esi]			; X1
		mov	bx,[esi+2]			; Y1
		mov	dx,[esi+4]			; X2
		mov	si,[esi+6]			; Y2

		call	FlatScanConvert
		pop	ecx esi
		add	esi,4
		dec	cx
		jnz	@@lp1

	; draw poly

		call	DrawFlatPoly
@@nodraw:	ret
DrawFlatClipped	endp


;================================================
; Draw clipped textured polygon
;

DrawTextClipped	proc
		mov	ecx,[NumPoints]			; number of pairs of coords
		test	cx,cx
		jz	@@nodraw

		mov	esi,offset CoordList		; get ptr to clipped coords

	; insert first coordinates for endpoints of last line

		mov	ax,[esi]			; get 1st coords
		mov	[esi+ecx*8],ax			; save at end of list
		mov	ax,[esi+2]
		mov	[esi+ecx*8+2],ax
		mov	ax,[esi+4]
		mov	[esi+ecx*8+4],ax
		mov	ax,[esi+6]
		mov	[esi+ecx*8+6],ax

	; create scan list

@@lp1:		push	esi ecx
		mov	ax,[esi+8]			; X1
		sal	eax,16
		mov	ax,[esi]			; X0

		mov	dx,[esi+8+2]			; Y1
		sal	edx,16
		mov	dx,[esi+2]			; Y0

		mov	ch,[esi+4]			; U0
		mov	cl,[esi+8+4]			; U1

		mov	bh,[esi+6]			; V0
		mov	bl,[esi+8+6]			; V1

		call	TextScanConvert
		pop	ecx esi
		add	esi,8
		dec	cx
		jnz	@@lp1

	; draw poly

		call	DrawTextPoly
@@nodraw:	ret
DrawTextClipped	endp


;================================================
; Scan convert Textured edge
;
; IN:	EAX = X1,X0 (AX=X0)
;	 EDX = Y1,Y0 (DX=Y0)
;	 BL = V1, BH = V0
;	 CL = U1, CH = U0
;

TextScanConvert	proc

	local	ddx:dword,ddu:dword,ddv:dword=LocalSize

		
		push	edi

		push	bp
		mov	bp,sp
		sub	sp,LocalSize

	; calculate height and check for -dy

		mov	edi,buffer_depth	; offset for min vals
		
		mov	si,dx
		ror	edx,16
		cmp	dx,si
		jz	@@SkipHorz			; shouldn't need to worry about horizontal lines

		jnc	@@l0

	; dy negative so vector is north-bound

		xor	edi,edi				; offset for max vals
		ror	eax,16				; swap X0 - X1
		mov	si,dx				; move other to si
		ror	edx,16				; swap Y0 - Y1
		ror	bx,8				; swap V0 - V1
		ror	cx,8				; swap U0 - V0


	; save initial values

@@l0:		push	ax bx cx

	; get height (dy)

		


		sub	dx,si				; dx = height
		add	di,si				; si = min Y
		cmp	si,[MinFillLine]
		jge	@@l0a

		mov	[MinFillLine],si

@@l0a:		shl	edx,16			;
		mov	dx,si				;edx = 16:16: miny:dy
		ror	edx,16

	; calculate dx

		mov	esi,eax
		shr	eax,16
		sub	ax,si				; dx
		pushf
		jnc	@@l1

		neg	ax

		test	dh,dh
		jnz	@@RealDivs

@@l1:		test	ah,ah				; dx>255 is special case
		jz	@@l1b

		push	ax				; 255/dy + (dx-255)/dy
		mov	ah,dl
		mov	al,255
		mov	esi,[eax*4+DivTable]		; ESI = 255/dy
		pop	ax
		sub	ax,255
		mov	ah,dl
		add	esi,[eax*4+DivTable]		; ESI = dx/dy
		jmp	short @@l1c

@@l1b:		mov	ah,dl				; need dx/dy
		mov	esi,[eax*4+DivTable]		; dx = 16.16 float
@@l1c:		popf
		jnc	@@l1a

		neg	esi
@@l1a:		mov	ddx,esi

	; calculate du

		sub	cl,ch				; get du
		pushf
		jnc	@@l2

		neg	cl
@@l2:		mov	ch,dl
		and	ecx,0ffffh
		mov	esi,[ecx*4+DivTable]		; du = 16.16 float
		popf
		jnc	@@l2a

		neg	esi
@@l2a:		mov	ddu,esi

	; calculate dv
		 
		sub	bl,bh
		pushf
		jnc	@@l3

		neg	bl
@@l3:		mov	bh,dl
		and	ebx,0ffffh
		mov	esi,[ebx*4+DivTable]		; dv = 16.16 float
		popf
		jnc	@@l3a

		neg	esi
@@l3a:		mov	ddv,esi

	; pick up init values

		pop	ax cx si
		and	eax,0ff00h
		sal	eax,8				; U0 (8.16)

		and	ecx,0ff00h
		sal	ecx,8				; V0 (8.16)

		sal	esi,16				; X0 (8.16)

	; update X, U, V
	; IN:	ESI=init X (9.16)
	;	EAX=init U (8.16)
	;	ECX=init V (8.16)
	;	EDI=0 or 200 for offset to min or max table
	; 	DH=size

@@loop:		ror	esi,16				; get int parts
		ror	eax,16
		ror	ecx,16

		mov	[MinTbl+edi*2],si		; update lists
		mov	[UMinTbl+edi],al
		mov	[VMinTbl+edi],cl

		ror	esi,16				; correct 
		ror	eax,16
		ror	ecx,16

		add	esi,ddx				; add in dx
		add	eax,ddu				; add in du
		add	ecx,ddv				; add in dv

		inc	edi
		dec	dx
		jnz	@@loop

@@RealDivs:
@@SkipHorz:
		mov	sp,bp
		pop	bp

		pop	edi
		ret
TextScanConvert	endp

if 0
;================================================
; Scan convert flat polygon
;
; IN:	AX=X1, DX=X2, BX=Y1, SI=Y2

; IN:	EAX = X1,X0 (AX=X0)
;	 DH = Y1, DL = Y0
;	 BL = V1, BH = V0
;	 CL = U1, CH = U0
;

FlatScanConvert	proc

	; fudge to scale the screen Y 0-239 to 0-199

		xor	ecx,ecx

	; end of fudge

		push	edi

	; calculate height and check for -dy

		mov	edi,buffer_depth	; offset for min vals

		cmp	si,bx
		jz	@@SkipHorz			; shouldn't need to worry about horizontal lines

		jnc	@@l0

	; dy negative so vector is north-bound

		xor	edi,edi				; offset for max vals
		xchg	ax,dx
		xchg	bx,si

	; save initial values

@@l0:		push	ax

	; get height (dy)

		sub	si,bx				; SI = height
		add	di,bx				; BX = min Y
		cmp	bx,[MinFillLine]
		jge	@@l0a

		mov	[MinFillLine],bx

@@l0a:

	; calculate dx

		sub	dx,ax				; dx
		pushf
		jnc	@@l1

		neg	dx

@@l1:		test	dh,dh				; dx>255 is special case
		jz	@@l1b

		push	ax				; 255/dy + (dx-255)/dy
		mov	ah,dh
		mov	al,255
		mov	esi,[eax*4+DivTable]		; ESI = 255/dy
		pop	ax
		sub	ax,255
		mov	ah,dh
		add	esi,[eax*4+DivTable]		; ESI = dx/dy
		jmp	short @@l1c

@@l1b:		mov	ah,dh				; need dx/dy
		mov	esi,[eax*4+DivTable]		; dx = 16.16 float
@@l1c:		popf
		jnc	@@l1a

		neg	esi
@@l1a:		mov	ddx,esi

	; pick up init values

		pop	ax cx si

	; update X, U, V
	; IN:	SI=init X (int)
	;	AX=init X (frac)
	;	BX=dx (int)
	;	CX=dx (frac)
	;	EDI=0 or 200 for offset to min or max table
	; 	DH=size

@@loop:		mov	[MinTbl+edi*2],si		; update lists
		add	ax,cx
		adc	si,bx				; add in dx
		inc	edi
		dec	dh
		jnz	@@loop

@@SkipHorz:
		pop	edi
		ret
FlatScanConvert	endp
endif

;================================================
; scan convert line
; AX=X1, DX=X2, BX=Y1, SI=Y2

FlatScanConvert	proc
		push	edi
		xor	edi,edi				; clear high word

	; fudge to scale the screen Y 0-239 to 0-199

	; make sure x1<=x2

		cmp	ax,dx
		jle	short @@fl1

		xchg	ax,dx				; swap pt1 and pt2
		xchg	bx,si

@@fl1:		mov	di,bx				; y1
		shl	di,1				; *2 for word table
		sub	dx,ax				; get delx
		mov	bp,2				; assume pos dely, 2 for word tables
		cmp	si,bx				; y2 >y1?
		jge	short @@fl2

		neg	bp				; no neg dely dir
		xchg	si,bx				; si= maxy bx=miny
@@fl2:
     		cmp	bx,[MinFillLine]		; lowest minmax table entry ?
		jge	short @@fl3			; no

		mov	[MinFillLine],bx

@@fl3:		sub	si,bx				; get abs dely
		cmp	si,dx				; dely >delx ?
		ja	short @@flinedy			; yes-go do it

		mov	cx,dx				; pix= delx+1, for loop
		inc	cx				; # pix +1

		mov	bx,cx				; get abs delx
		shr	bx,1				; rmdr=-delx/2
		neg	bx				; all set for draw
		jmp	short @@flin_1

@@flinfixy:	cmp	ax,[edi.MaxTbl]			; greater than current max ?
		jle	short @@fl4

		mov	[edi.MaxTbl],ax

@@fl4:		sub	bx,dx				; reset remainder, rmdr=rmdr-delx
		dec	cx				; that it ?
		je	short @@flin_2			; yes
	
		inc	ax				; next x
		add	di,bp				; next table entry

@@flin_1:	cmp	ax,[edi.MinTbl]			; less than current min ?
		jge	short @@fl5
	
		mov	[edi.MinTbl],ax

@@fl5:		add	bx,si				; rmdr=rmdr+dely
		jns	short @@flinfixy		; next y line
	
		inc	ax				; next x pixel
		dec	cx
		jnz	short @@fl5			; keep going

	; fell out from done in x-check for last max
	
		dec	ax				; went one too far
		cmp	ax,[edi.MaxTbl]			; greater than current max ?
		jle	short @@fl6
	
		mov	[edi.MaxTbl],ax
@@fl6:
@@flin_2:
		pop	edi
		ret

	; ax= xlo
	; dx= abs delx
	; si= abs dely
	; bx= ylo
	; di = y1 min/max

@@flinedy:	mov	cx,si
		inc	cx				; # lines+1
		mov	bx,cx				; rmdr= -dely/2
		shr	bx,1
		neg	bx
		jmp	short @@flin_3

@@flin_4:	dec	cx				; that it ?
		je	short @@flin_5			; yes
	
		sub	bx,si				; rmdr=rmdr-dely
		inc	ax				; next x
		add	di,bp				; next table entry

@@flin_3:	cmp	ax,[edi.MinTbl]			; less than min ?
		jge	short @@f0
	
		mov	[edi.MinTbl],ax

@@f0:		cmp	ax,[edi.MaxTbl]			; greater than cur max ?
		jle	short @@f00
	
		mov	[edi.MaxTbl],ax

@@f00:		add	bx,dx				; rmdr=rmdr+delx
		jns	short @@flin_4
	
		add	di,bp
		dec	cx
		jnz	short @@flin_3

@@flin_5:
		pop	edi
		ret


vertfline:	cmp	cx,bx
		jge	short @@v0

		xchg	bx,cx

	; clip to y bounds
	; bx= lo,cx=hi

@@v0: 		sub	cx,bx				; # lines
		test	cx,cx
		jz	short @@vl_out			; dont put a single side-messes up top or bottom

		cmp	bx,[MinFillLine]		; lowest minmax table entry ?
		jge	short @@v3			; no
	
		mov	[MinFillLine],bx

@@v3:		inc	cx				; + 1 for loop
		shl	bx,1				; word tables

@@vfl_1:	cmp	ax,[ebx.MinTbl]			; less than min ?
		jge	short @@v4
	
		mov	[ebx.MinTbl],ax

@@v4:		cmp	ax,[ebx.MaxTbl]			; greater than cur max ?
		jle	short @@v5				
	
		mov	[ebx.MaxTbl],ax

@@v5:		add	ebx,2				; next entry
		dec	cx
		jnz	short @@vfl_1

@@vl_out:	ret

FlatScanConvert	endp


;================================================
; Draw current flat polygon into buffer
;

DrawFlatPoly	proc
		cmp	[Transluscency],0
		jnz	DrawFlatTPoly

		xor	ecx,ecx				; clear top half of regs
		xor	ebx,ebx
		xor	esi,esi

		mov	ax,[Colour]			; get colour into EAX
		shl	eax,16
		mov	ax,[Colour]

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pf1				; nothing to draw

		shl	esi,1				; word table

	; draw 1 line

@@pf2:		mov	bx,[esi.MinTbl]
		mov	edi,[esi*2+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pf1

		add	edi,[_CurBufferJ]		;jed
		add	edi,ebx				; add in X start
		mov	[edi],al			; write out at least one byte
		inc	edi
		mov	cx,[esi.MaxTbl]			; get length

		cmp	cx,08001h				;Not Set Invalid AJY
		je	@@pf1

		sub	cx,bx
		jz	short @@pfnxt

		cmp	cx,8
		jb	@@shortline

	; long line, word align

@@longline:	test	di,1				; odd byte ?
		jz	@@pfl1				; no-even -keep going

		stosb
		dec	cx

	; long line, dword align

@@pfl1:		test	di,2				; odd word ?
		jz	@@pfl2				; no-even -keep going

		stosw
		sub	cx,2

	; We are now dword aligned

	; CX=length, get into num DWORD's and fill

@@pfl2:		push	ecx
		shr	cx,2
		rep	stosd
		pop	ecx

	; complete any odd bytes

		and	cx,3
@@shortline:	rep	stosb

@@pfnxt:	mov	word ptr [esi+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi+MaxTbl],08001h
		add	si,2				; next line
		cmp	si,_WinHeightX2 ;CHANGE LATER
		jb	@@pf2
			  
@@pf1:		mov	word ptr [MinFillLine],07fffh
		ret
DrawFlatPoly	endp


;================================================
; Draw current flat polygon into buffer
; Transluscent

DrawFlatTPoly	proc
		xor	ecx,ecx				; clear top half of regs
		xor	ebx,ebx
		xor	esi,esi
		xor	eax,eax

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pfdne				; nothing to draw

		shl	esi,1				; word table

		mov	ax,[Transluscency]		; get index into table
		dec	ax
		sal	eax,8

	; draw 1 line

@@pf2:		mov	bx,[esi.MinTbl]
		mov	edi,[esi*2+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pfdne

		add	edi,[_CurBufferJ]		;jed
		add	edi,ebx				; add in X start

		mov	cx,[esi.MaxTbl]			; get length

		cmp	cx,08001h				;Not Set Invalid AJY
		je	@@pfdne



		inc	cx				; write out at least 1 byte

		sub	cx,bx
		jz	short @@pfnxt

@@pf0:		mov	al,[edi]
		mov	al,[eax+_DepthFadeTble]
		mov	[edi],al
		inc	edi
		dec	cx
		jnz	@@pf0

@@pfnxt:	mov	word ptr [esi+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi+MaxTbl],08001h
		add	si,2				; next line
		cmp	si,_WinHeightX2
		jb	@@pf2
			  
@@pfdne:	mov	word ptr [MinFillLine],07fffh
		ret
DrawFlatTPoly	endp


;================================================
; Draw current textured polygon into buffer
; non-transparent

DrawTextPoly	proc
	local	uFrac:dword,vFrac:dword=LocalSize
		
		cmp	[DepthFade],0
		jnz	DrawFadePoly

		cmp	[Transluscency],0
;		jnz	DrawTransPoly
		jnz	DrawTrlusPoly		;AJY want translucency

		push	bp
		mov	bp,sp
		sub	sp,LocalSize

		xor	esi,esi

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pf1				; nothing to draw

	; draw 1 line

@@pf2:		xor	ebx,ebx
		xor	edx,edx
		xor	eax,eax

		push	esi				; save line ptr
		mov	bx,[esi*2+MinTbl]
		mov	edi,[esi*4+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pf1

		add	edi,[_CurBufferJ]		; EDI->buffer	;jed

		mov	ax,[esi*2+MaxTbl]		; get dx
		cmp	ax,08001h
		je	@@pf1

		add	edi,ebx				; add in X start
		sub	ax,[esi*2+MinTbl]		; AX=width (dx)
		js	@@pfnxt

	; get width

		mov	cx,ax				; get width into CX
		dec	cx				; -1 for jns loop
		sal	ecx,16				; move width into high word
		sal	eax,8				; dx

	; calculate dv/dx, SI=frac, CH=int

		mov	al,[esi+VMaxTbl]		; BH=Vright
		sub	al,[esi+VMinTbl]		; BH=dv
		pushf
		jnc	@@s3

		neg	al
@@s3:		mov	edx,[eax*4+DivTable]		; EBX=dv/dx (16.16)
		popf
		jnc	@@s4

		neg	edx
@@s4:		mov	vFrac,edx
		shr	edx,8	  
		mov	ch,dh

	; calculate du/dx, DX=frac, CL=int

		mov	al,[esi+UMaxTbl]		; DH=UMax
		sub	al,[esi+UMinTbl]		; DH=du
		pushf
		jnc	@@s1

		neg	al
@@s1:		mov	edx,[eax*4+DivTable]		; EDX=du/dx (16.16)
		popf
		jnc	@@s2

		neg	edx

@@s2:		;sal	edx,1  ;for 2X doubling
		mov	uFrac,edx
		shr	edx,8
		mov	cl,dh

	; pick up texture start/end points and tPage

		mov	ebx,[tPage]
		mov	bl,[esi+UMinTbl]
		mov	bh,[esi+VMinTbl]

	; copy 1 line of texture
	; EBX high word = tpage
	; EDI->buffer
	; ECX (high word) = width (pixels)
	; CH:vfrac = dv/dx (8.16 fp)
	; CL:ufrac = du/dx (8.16 fp)
	; BL = initial u
	; BH = initial v
		mov esi,vFrac
		mov edx,uFrac
		shl esi,16
		shl edx,16
		mov vFrac, esi
		mov	uFrac, edx

		xor	esi,esi
		xor	edx,edx
		dec	edi
;		dec	edi ;for 2X texturing

@@loop:
		mov	al,[ebx+_VRam]			; pick up texture
;		mov	ax,dword ptr[ebx+_VRam]			; pick up texture;for 2X doubling
		add	edx,uFrac			; u frac
		adc	bl,cl				; u int
		inc	edi
;		add	edi,2 				;for 2X doubling
		add	esi,vFrac			; v frac
		adc	bh,ch				; v int
;		sub	ecx,20000h			; dec loop counter ;for 2X doubling
		sub	ecx,10000h			; dec loop counter
		mov	[edi],al			; and store texture
;		mov	[edi],ax			; and store texture ;for 2X doubling
		jns	@@loop

@@pfnxt:	pop	esi
		mov	word ptr [esi*2+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi*2+MaxTbl],08001h
		inc	si				; next line
		cmp	si,_WinHeight
		jb	@@pf2
			  
@@pf1:		mov	word ptr [MinFillLine],07fffh
		mov	sp,bp
		pop	bp
		ret
DrawTextPoly	endp


;================================================
; Draw transluscent textured polygon into buffer
; black pixels used for transluscency check

DrawTrlusPoly	proc

	local	uFrac:dword,vFrac:dword,Fade:dword=LocalSize

		cmp	[Transluscency],1
		jz	DrawTransPoly

		push	bp
		mov	bp,sp
		sub	sp,LocalSize

		xor	esi,esi

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pf1				; nothing to draw

		xor	eax,eax
		mov	ax,[Transluscency]		; get index into table
		dec	ax
		sal	eax,8
		mov	Fade,eax

	; draw 1 line

@@pf2:		xor	ebx,ebx
		xor	edx,edx
		xor	eax,eax

		push	esi				; save line ptr
		mov	bx,[esi*2+MinTbl]
		mov	edi,[esi*4+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pf1

		add	edi,[_CurBufferJ]		; EDI->buffer ;jed

		mov	ax,[esi*2+MaxTbl]		; get dx

		cmp	ax,08001h				;Reject Line as invalid AJY
		je	@@pf1

		add	edi,ebx				; add in X start
		sub	ax,[esi*2+MinTbl]		; AX=width (dx)
		js	@@pfnxt

	; get width

		mov	cx,ax				; get width into CX
		dec	cx				; -1 for jns loop
		sal	ecx,16				; move width into high word
		sal	eax,8				; dx

	; calculate dv/dx, SI=frac, CH=int

		mov	al,[esi+VMaxTbl]		; BH=Vright
		sub	al,[esi+VMinTbl]		; BH=dv
		pushf
		jnc	@@s3

		neg	al
@@s3:		mov	edx,[eax*4+DivTable]		; EBX=dv/dx (16.16)
		popf
		jnc	@@s4

		neg	edx
@@s4:		mov	vFrac,edx
		shr	edx,8
		mov	ch,dh

	; calculate du/dx, DX=frac, CL=int

		mov	al,[esi+UMaxTbl]		; DH=UMax
		sub	al,[esi+UMinTbl]		; DH=du
		pushf
		jnc	@@s1

		neg	al
@@s1:		mov	edx,[eax*4+DivTable]		; EDX=du/dx (16.16)
		popf
		jnc	@@s2

		neg	edx
@@s2:		mov	uFrac,edx
		shr	edx,8
		mov	cl,dh

	; pick up texture start/end points and tPage

		mov	ebx,[tPage]
		mov	bl,[esi+UMinTbl]
		mov	bh,[esi+VMinTbl]

	; copy 1 line of texture
	; EBX high word = tpage
	; EDI->buffer
	; ECX (high word) = width (pixels)
	; CH:vfrac = dv/dx (8.16 fp)
	; CL:ufrac = du/dx (8.16 fp)
	; BL = initial u
	; BH = initial v

		mov	eax,Fade
		mov esi,vFrac
		mov edx,uFrac
		shl esi,16
		shl edx,16
		mov vFrac, esi
		mov	uFrac, edx

		xor	esi,esi
		xor	edx,edx
		dec	edi

@@loop:		mov	al,[ebx+_VRam]			; pick up texture
		inc	edi
		add	edx,uFrac			; u frac
		adc	bl,cl				; u int
		add	esi,vFrac			; v frac
		adc	bh,ch				; v int
		test	al,al				; if black then don't draw AJY
		jz	@@skip

		mov	al,[edi]			; pick up original colour
		mov	al,[eax+_DepthFadeTble]

		mov	[edi],al			; and store texture
@@skip:		sub	ecx,10000h			; dec loop counter moved skip down AJY
		jns	@@loop

@@pfnxt:	pop	esi
		mov	word ptr [esi*2+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi*2+MaxTbl],08001h
		inc	si				; next line
		cmp	si,_WinHeight
		jb	@@pf2
			  
@@pf1:		mov	word ptr [MinFillLine],07fffh
		mov	sp,bp
		pop	bp
		ret
DrawTrlusPoly	endp


;================================================
; Draw current textured polygon into buffer
; allow transparency
;

DrawTransPoly	proc

	local	uFrac:dword,vFrac:dword=LocalSize

		push	bp
		mov	bp,sp
		sub	sp,LocalSize

		xor	esi,esi

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pf1				; nothing to draw

	; draw 1 line

@@pf2:		xor	ebx,ebx
		xor	edx,edx
		xor	eax,eax

		push	esi				; save line ptr
		mov	bx,[esi*2+MinTbl]
		mov	edi,[esi*4+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pf1

		add	edi,[_CurBufferJ]		; EDI->buffer ;jed

		mov	ax,[esi*2+MaxTbl]		; get dx

		cmp	ax,08001h				;Not Set Invalid AJY
		je	@@pf1


		add	edi,ebx				; add in X start
		sub	ax,[esi*2+MinTbl]		; AX=width (dx)
		js	@@pfnxt

	; get width

		mov	cx,ax				; get width into CX
		dec	cx				; -1 for jns loop
		sal	ecx,16				; move width into high word
		sal	eax,8				; dx

	; calculate dv/dx, SI=frac, CH=int

		mov	al,[esi+VMaxTbl]		; BH=Vright
		sub	al,[esi+VMinTbl]		; BH=dv
		pushf
		jnc	@@s3

		neg	al
@@s3:		mov	edx,[eax*4+DivTable]		; EBX=dv/dx (16.16)
		popf
		jnc	@@s4

		neg	edx
@@s4:		mov	vFrac,edx			;still shl 16
		shr	edx,8
		mov	ch,dh

	; calculate du/dx, DX=frac, CL=int

		mov	al,[esi+UMaxTbl]		; DH=UMax
		sub	al,[esi+UMinTbl]		; DH=du
		pushf
		jnc	@@s1

		neg	al
@@s1:		mov	edx,[eax*4+DivTable]		; EDX=du/dx (16.16)
		popf
		jnc	@@s2

		neg	edx
@@s2:		mov	uFrac,edx		; still shl 16
		shr	edx,8
		mov	cl,dh

	; pick up texture start/end points and tPage

		mov	ebx,[tPage]
		mov	bl,[esi+UMinTbl]
		mov	bh,[esi+VMinTbl]

	; copy 1 line of texture
	; EBX high word = tpage
	; EDI->buffer
	; ECX (high word) = width (pixels)
	; CH:vfrac = dv/dx (8.16 fp)
	; CL:ufrac = du/dx (8.16 fp)
	; BL = initial u
	; BH = initial v

		mov esi,vFrac
		mov edx,uFrac
		shl esi,16
		shl edx,16
		mov vFrac, esi
		mov	uFrac, edx
		xor	esi,esi
		xor	edx,edx
		dec	edi

@@loop:
		mov	al,[ebx+_VRam]			; pick up texture
		add	edx,uFrac			; u frac
		adc	bl,cl				; u int
		inc	edi
		add	esi,vFrac			; v frac
		adc	bh,ch				; v int
		test	al,al
		jz	@@SkipTrans

		mov	[edi],al			; and store texture

@@SkipTrans:	sub	ecx,10000h			; dec loop counter
		jns	@@loop

@@pfnxt:	pop	esi
		mov	word ptr [esi*2+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi*2+MaxTbl],08001h
		inc	si				; next line
		cmp	si,_WinHeight
		jb	@@pf2
			  
@@pf1:		mov	word ptr [MinFillLine],07fffh
		mov	sp,bp
		pop	bp
		ret
DrawTransPoly	endp


;================================================
; Draw current textured polygon into buffer
; allow depth fading
;

DrawFadePoly	proc

	local	uFrac:dword,vFrac:dword,Fade:dword=LocalSize

		push	bp
		mov	bp,sp
		sub	sp,LocalSize

		xor	esi,esi

		mov	si,[MinFillLine]
		cmp	si,_WinHeight
		jge	@@pf1				; nothing to draw

		xor	eax,eax
		mov	ax,[DepthFade]			; get index into table
		sal	eax,8
		mov	Fade,eax

	; draw 1 line

@@pf2:		xor	ebx,ebx
		xor	edx,edx
		xor	eax,eax

		push	esi				; save line ptr
		mov	bx,[esi*2+MinTbl]
		mov	edi,[esi*4+LineStart]		; get address of start of line
		cmp	bx,07fffh			; done?
		jz	@@pf1

		add	edi,[_CurBufferJ]		; EDI->buffer ;jed

		mov	ax,[esi*2+MaxTbl]		; get dx

		cmp	ax,08001h				;Not Set Invalid AJY
		je	@@pf1


		add	edi,ebx				; add in X start
		sub	ax,[esi*2+MinTbl]		; AX=width (dx)
		js	@@pfnxt

	; get width

		mov	cx,ax				; get width into CX
		dec	cx				; -1 for jns loop
		sal	ecx,16				; move width into high word
		sal	eax,8				; dx

	; calculate dv/dx, SI=frac, CH=int

		mov	al,[esi+VMaxTbl]		; BH=Vright
		sub	al,[esi+VMinTbl]		; BH=dv
		pushf
		jnc	@@s3

		neg	al
@@s3:		mov	edx,[eax*4+DivTable]		; EBX=dv/dx (16.16)
		popf
		jnc	@@s4

		neg	edx
@@s4:		mov	vFrac,edx	;vFrac needs to shift left 16
		shr	edx,8
		mov	ch,dh

	; calculate du/dx, DX=frac, CL=int

		mov	al,[esi+UMaxTbl]		; DH=UMax
		sub	al,[esi+UMinTbl]		; DH=du
		pushf
		jnc	@@s1

		neg	al
@@s1:		mov	edx,[eax*4+DivTable]		; EDX=du/dx (16.16)
		popf
		jnc	@@s2

		neg	edx
@@s2:		mov	uFrac,edx		;uFrac needs to shift left 16
		shr	edx,8
		mov	cl,dh

	; pick up texture start/end points and tPage

		mov	ebx,[tPage]
		mov	bl,[esi+UMinTbl]
		mov	bh,[esi+VMinTbl]
		mov	edx,uFrac
		mov	esi,vFrac
		shl	edx,16
		shl esi,16
		mov uFrac,edx
		mov	vFrac,esi

	; copy 1 line of texture
	; EBX high word = tpage
	; EDI->buffer
	; ECX (high word) = width (pixels)
	; CH:vfrac = dv/dx (8.16 fp)
	; CL:ufrac = du/dx (8.16 fp)
	; BL = initial u
	; BH = initial v

		mov	eax,Fade
		
		xor	esi,esi
		xor	edx,edx
		dec	edi
@@loop:
		mov	al,[ebx+_VRam]			; pick up texture
		add	edx,uFrac			; u frac
		adc	bl,cl				; u int
		mov	al,[eax+_DepthFadeTble]		; get depth fade value
		inc	edi
		add	esi,vFrac			; v frac
		adc	bh,ch				; v int
		mov	[edi],al			; store texture
		sub	ecx,10000h			; dec loop counter
		jns	@@loop

@@pfnxt:	pop	esi
		mov	word ptr [esi*2+MinTbl],07fffh	; reset line table for next time
		mov	word ptr [esi*2+MaxTbl],08001h
		inc	si				; next line
		cmp	si,_WinHeight
		jb	@@pf2
			  
@@pf1:		mov	word ptr [MinFillLine],07fffh
		mov	sp,bp
		pop	bp
		ret
DrawFadePoly	endp



;----------------------------------------------------------
; Flag OK to copy buffer
; If timer not installed then will also copy buffer

GSwap_		proc	near
		push	eax

@@Wait:		cmp	[CopyFlag],1			; wait for int to copy prev buffer
		jz	@@Wait

	; xchg buffer ptrs

		mov	eax,[_CurBuffer]
		xchg	eax,[_CurBuffer+4]
		mov	[_CurBuffer],eax

	; flag OK to copy

		inc	[CopyFlag]

	; if timer not installed then copy buffer to screen now

		call	CopyBuffer_

@@dne:		pop	eax
		ret
GSwap_		endp


;----------------------------------------------------------
; Copy buffer to screen
;

CopyBuffer_	proc	near
		push	edi esi ecx

		cmp	[CopyFlag],0
		jz	@@rtn

		dec	[CopyFlag]

		cld
		xor	ecx,ecx
		mov	cx,(320*200)/4
		mov	esi,[_CurBuffer+4]
;		mov	edi,_Render_Buffer
		mov	edi,0a0000h
		rep	movsd

@@rtn:		pop	ecx esi edi
		ret
CopyBuffer_	endp


;----------------------------------------------------------
; Clear Current Buffer
;
; IN: AL = colour no to clear to

ClearBuf_	proc	near
		push	edi ecx eax
		mov	ah,al
		mov	cx,ax
		sal	eax,16
		mov	ax,cx
		cld
		mov	edi,[_CurBuffer]
		mov	ecx,320*200/4
		rep	stosd
		pop	eax ecx edi
		ret
ClearBuf_	endp

ClearGameBuf_	proc	near
		push	edi ecx eax
		mov	ah,al
		mov	cx,ax
		sal	eax,16
		mov	ax,cx
		cld
		mov	edi,[_CurBuffer]
		mov	ecx,320*133/4
		rep	stosd
		mov	edi,[_CurBuffer]	;Bodge for bottom line
		add	edi,320*199
		xor	eax, eax
		mov	ecx,320/4
		rep stosd
		mov edi,[_CurBuffer]
		add	edi,319
		mov	ecx,200
CGloop1:
		mov	byte ptr [edi],al
		add	edi,320
		dec	ecx
		jnz	CGloop1
		pop	eax ecx edi
		ret
ClearGameBuf_	endp


WaitVbl_	proc	near
		push	dx
   		push	ax

vloop001:	mov	dx,03dah
		in	al,dx
		test	al,1000b
		jnz	vloop001

vloop002:	mov	dx,03dah
		in	al,dx
		test	al,1000b
		jz	vloop002

		pop	ax
		pop	dx
		ret
WaitVbl_	endp


Pen_		proc
		mov	bh,dl		;dl=green
			 
		mov	edx,03c8h    	;al = colour
		out	dx,al
		jmp	$+2
		jmp	$+2
		inc	dx
		;
		mov	al,cl		;cl = red
		out	dx,al
		jmp	$+2
		;
		mov	al,bh		;bh = green
		out	dx,al
		jmp	$+2
		;
		mov	al,bl		;bl = blue
		out	dx,al
		jmp	$+2
		ret
Pen_		endp

;get a random number in AX
;Randy_		proc	near
		;mov	edx,eax
		;mov	eax,[RandNum]
		;mov	ebx,eax
		;ror	ebx,2
		;add	eax,ebx
		;ror	ebx,5
		;add	eax,ebx
		;inc	eax
		;mov	[RandNum],eax
		;mul	edx
		;mov	eax,edx
		;and eax, 7FFFh
		;ret
;Randy_		endp


stuffem256_	proc near
	mov	ax,[esi]
	and	ax,1fh
	mov	bx,ax
	mov	ax,[esi+2]
	and	ax,1fh
	add	bx,ax
	mov	ax,[esi+512]
	and	ax,1fh
	add	bx,ax
	mov	ax,[esi+514]
	and	ax,1fh
	add	ax,bx
	shr	ax,2
	mov	dx,ax

	mov	ax,[esi]
	and	ax,3e0h
	mov	bx,ax
	mov	ax,[esi+2]
	and	ax,3e0h
	add	bx,ax
	mov	ax,[esi+512]
	and	ax,3e0h
	add	bx,ax
	mov	ax,[esi+514]
	and	ax,3e0h
	add	ax,bx
	shr	ax,2
	and	ax,3e0h
	or	dx,ax

	movzx	eax,WORD PTR[esi]
	and	eax,7c00h
	mov	ebx,eax
	movzx	eax,WORD PTR[esi+2]
	and	eax,7c00h
	add	ebx,eax
	movzx	eax,WORD PTR[esi+512]
	and	eax,7c00h
	add	ebx,eax
	movzx	eax,WORD PTR[esi+514]
	and	eax,7c00h
	add	eax,ebx
	shr	eax,2
	and	eax,7c00h
	or	ax,dx
	ret
	endp

		end

