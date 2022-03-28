;;-----------------------------------------------------------------------//
;; IPX EVENT HANDLER Network assembler routine.
;; Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
;;-----------------------------------------------------------------------//

								.386
								.model			small
								.code

;;-----------------------------------------------------------------------//

_text16							segment byte public use16 'CODE'
								assume cs:_text16

;;-----------------------------------------------------------------------//

public							IPX_RealModeESR_
public							_esr_start
public							_esr_receive_head
public							_esr_receive_tail
public							_esr_receive_buffer
public							_esr_handler

;;-----------------------------------------------------------------------//

ESR_RECEIVE_BUFFER_SIZE			equ		16*1024
ESR_HANDLER_BUFFER_SIZE			equ		4*1024
ESR_DATA_PSIZE					equ		(ESR_RECEIVE_BUFFER_SIZE + 16) / 16

;;-----------------------------------------------------------------------//

;esr structure
esr_start				equ		0		;dd		?
esr_size				equ		4		;dd		?
esr_receive_head		equ		8		;dd		?
esr_receive_tail		equ		12		;dd		?
esr_receive_buffer		equ		16		;db (ESR_RECEIVE_BUFFER_SIZE) dup (?)
esr_handler_buffer		equ		16 + (ESR_RECEIVE_BUFFER_SIZE)	;db (ESR_HANDLER_BUFFER_SIZE) dup (?)

;ecb_structure					struc
link					equ		0		;dd		?
esr_offset				equ		4		;dw		?
esr_segment				equ		6		;dw		?
inuse					equ		8		;db		?
code					equ		9		;db		?
socket					equ		10		;dw		?
reserved_for_ipx		equ		12		;dd		?
reserved_for_driver1	equ		16		;dd		?
reserved_for_driver2	equ		20		;dd		?
reserved_for_driver3	equ		24		;dd		?
localaddress			equ		28		;db		10 dup (?)
fragments				equ		34		;dw		?
frag0_offset			equ		36		;dw		?
frag0_segment			equ		38		;dw		?
frag0_size				equ		40		;dw		?
frag1_offset			equ		42		;dw		?
frag1_segment			equ		44		;dw		?
frag1_size				equ		46		;dw		?

;id_structure
PktSize					equ		0		;dd		?
GameID					equ		4		;db		4 dup (?)
PktType					equ		8		;dd		?
PktSyncCounter			equ		12		;dd		?

;;-----------------------------------------------------------------------//
;; Relative Start of Data Area
;;-----------------------------------------------------------------------//

_esr_start						dd		0
_esr_size						dd		(4*4) + (ESR_RECEIVE_BUFFER_SIZE) + (ESR_HANDLER_BUFFER_SIZE)
_esr_receive_head				dd		0
_esr_receive_tail				dd		0
_esr_receive_buffer				db		(ESR_RECEIVE_BUFFER_SIZE) dup (?)
_esr_handler					equ		IPX_RealModeESR_

;;-----------------------------------------------------------------------//

								; all regs saved by IPX
IPX_RealModeESR_				proc far
								; es:si -> ECB (si = 0)
								; al 	-> caller id
								; interrupts disabled

								; check caller is AES
								;test	al,0FFH
								;je		esr_end

								; point source	ds -> ECB structure
								mov		dx, es
								mov		ds, dx
								mov		gs, dx

								; point dest 	es -> ESR structure
								mov		ax,cs
								sub		ax,ESR_DATA_PSIZE
								mov		es,ax

								; check for WIPEOUT packet
								mov		ax, word ptr ds:[frag1_segment]
								mov		fs, ax
								cmp		byte ptr fs:[GameID + 0], 'W'
								jne		reset_ecb
								cmp		byte ptr fs:[GameID + 1], 'I'
								jne		reset_ecb
								cmp		byte ptr fs:[GameID + 2], 'P'
								jne		reset_ecb
								cmp		byte ptr fs:[GameID + 3], 'E'
								jne		reset_ecb

								inc		dword ptr es:[esr_start]		;counter checking for no of WIPEOUT packets processed

								; decode data into circular buffer
								mov		eax, dword ptr es:[esr_receive_tail]
								add		ax, word ptr fs:[PktSize]
								mov		ecx, ESR_RECEIVE_BUFFER_SIZE
								cmp		eax, ecx
								jle		no_wrap_copy

wrap_copy:						cld
								; calc size to end
								sub		ecx, dword ptr es:[esr_receive_tail]	; sizetoend = ESR_RECEIVE_BUFFER_SIZE - esr_receive_tail
								mov		ebx, ecx								; ebx = sizetoend
								mov		esi, 0
								mov		si, word ptr ds:[frag1_offset]
								mov		ax, word ptr ds:[frag1_segment]
								mov		ds, ax									; ds:esi = fragment2 data
								mov		edi, esr_receive_buffer
								add		edi, dword ptr es:[esr_receive_tail]	; es:edi = receive buffer ptr
rep								movsb
								mov		ecx, 0
;								mov		cx, word ptr gs:[frag1_size]
								mov		cx, word ptr fs:[PktSize]
								sub		ecx, ebx
								mov		edi, esr_receive_buffer
								mov		dword ptr es:[esr_receive_tail], ecx
rep								movsb
								; wrap test flag
								mov		dword ptr es:[esr_start], 58H
								jmp		reset_ecb

no_wrap_copy:
								cld
								mov		ecx, 0
								mov		cx, word ptr fs:[PktSize]
								mov		esi, 0
								mov		si, word ptr ds:[frag1_offset]
								mov		ax, word ptr ds:[frag1_segment]
								mov		ds, ax									; ds:esi = fragment2 data
								mov		edi, esr_receive_buffer
								add		edi, dword ptr es:[esr_receive_tail]	; es:edi = receive buffer ptr
								add		dword ptr es:[esr_receive_tail], ecx
rep								movsb
								cmp		dword ptr es:[esr_receive_tail], ESR_RECEIVE_BUFFER_SIZE
								jl		reset_ecb
								mov		dword ptr es:[esr_receive_tail], 0

								; assign ECB to listen for a new packet (ES:SI -> ECB)
reset_ecb:						mov		esi,0
								mov		ax,gs
								mov 	es,ax
								mov		ebx, 4
								int		7Ah

								; esr terminates here
esr_end:						; all regs restored by IPX
								retf
IPX_RealModeESR_				endp
_esr_end						db		0

;;-----------------------------------------------------------------------//

_text16							ends

;;-----------------------------------------------------------------------//

								end

;;-----------------------------------------------------------------------//

