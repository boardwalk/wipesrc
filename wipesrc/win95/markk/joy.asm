.386P

.model flat

	public	ReadJoystick_


.DATA

	extrn	_JoyVal:word

axisnum		db	0


.CODE

ReadJoystick_	proc
;		mov	word ptr _JoyVal, 0
;		mov	[axisnum], al

;		cli          			; Disable interrupts so they don't interfere with timing
;		mov 	dx, 0201h   		; Write dummy byte to joystick port
;		out 	dx, al

@@joystickloop:
;		inc 	_JoyVal              	; Add one to JoyVal
;		cmp 	_JoyVal, 0FFFFh       	; Check for time out
;		je 	@@done
;		in 	al, dx			; Get joystick port value
;		and 	al, axisnum        	; Test the appropriate bit
;		jne 	@@joystickloop

@@done:
;		sti                    		; Enable interrupts again
ReadJoystick_	endp

		end

