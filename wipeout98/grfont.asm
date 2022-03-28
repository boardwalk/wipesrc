;GRTEXT font bitmap data

	.486p
	.model		flat
	.data

	PUBLIC		_DebugFontTable
	PUBLIC		_NumericFontTable

;Font Bitmap for graphics mode (8x8 character cells)
_DebugFontTable		DB		8 DUP (0)		; NUL
					DB		8 DUP (0)		; SOH
					DB		8 DUP (0)		; STX
					DB		8 DUP (0)		; ETX
					DB		8 DUP (0)		; EOT
					DB		8 DUP (0)		; ENQ
					DB		8 DUP (0)		; ACK
					DB		8 DUP (0)		; BEL
					DB		8 DUP (0)		; BS
					DB		8 DUP (0)		; HT
					DB		8 DUP (0)		; LF
					DB		8 DUP (0)		; VT
					DB		8 DUP (0)		; FF
					DB		8 DUP (0)		; CR
					DB		8 DUP (0)		; SO
					DB		8 DUP (0)		; SI
					DB		8 DUP (0)		; DLE
					DB		8 DUP (0)		; DC1
					DB		8 DUP (0)		; DC2
					DB		8 DUP (0)		; DC3
					DB		8 DUP (0)		; DC4
					DB		8 DUP (0)		; NAK
					DB		8 DUP (0)		; SYN
					DB		8 DUP (0)		; ETB
					DB		8 DUP (0)		; CAN
					DB		8 DUP (0)		; EM
					DB		8 DUP (0)		; SUB
					DB		8 DUP (0)		; ESC
					DB		8 DUP (0)		; FS
					DB		8 DUP (0)		; GS
					DB		8 DUP (0)		; RS
					DB		8 DUP (0)		; US
					DB		8 DUP (0)		; SPACE
					DB		8 DUP (0)		; !
					DB		8 DUP (0)		; "
					DB		8 DUP (0)		; #
					DB		8 DUP (0)		; $
					DB		8 DUP (0)		; %
					DB		8 DUP (0)		; &
					DB		8 DUP (0)		; '
					DB		8 DUP (0)		; (
					DB		8 DUP (0)		; )
					DB		8 DUP (0)		; *
					DB		8 DUP (0)		; +
					DB		8 DUP (0)		; ,
					DB		8 DUP (0)		; -
					DB		8 DUP (0)		; .
					DB		8 DUP (0)		; /

_NumericFontTable	DB		00000000b		; 0
					DB		00111100b
					DB		01000010b
					DB		01000110b
					DB		01001010b
					DB		01010010b
					DB		01100010b
					DB		00111100b

					DB		00000000b		; 1
					DB		00001000b
					DB		00111000b
					DB		00001000b
					DB		00001000b
					DB		00001000b
					DB		00001000b
					DB		00111110b

					DB		00000000b		; 2
					DB		00011100b
					DB		00100010b
					DB		00100010b
					DB		00000100b
					DB		00011000b
					DB		00110010b
					DB		00111110b

					DB		00000000b		; 3
					DB		00011100b
					DB		00100010b
					DB		00000100b
					DB		00000100b
					DB		00100010b
					DB		00100010b
					DB		00011100b

					DB		00000000b		; 4
					DB		00000100b
					DB		00001100b
					DB		00010100b
					DB		00111110b
					DB		00000100b
					DB		00000100b
					DB		00000100b

					DB		00000000b		; 5
					DB		00111110b
					DB		00100000b
					DB		00111100b
					DB		00000010b
					DB		00100010b
					DB		00100010b
					DB		00011100b

					DB		00000000b		; 6
					DB		00001110b
					DB		00010000b
					DB		00100000b
					DB		00111100b
					DB		00100010b
					DB		00100010b
					DB		00011100b

					DB		00000000b		; 7
					DB		00111110b
					DB		00000010b
					DB		00000010b
					DB		00000100b
					DB		00001000b
					DB		00001000b
					DB		00001000b

					DB		00000000b		; 8
					DB		00011100b
					DB		00100010b
					DB		00100010b
					DB		00011100b
					DB		00100010b
					DB		00100010b
					DB		00011100b

					DB		00000000b		; 9
					DB		00011100b
					DB		00100010b
					DB		00100010b
					DB		00011110b
					DB		00000100b
					DB		00001000b
					DB		00010000b

					DB		00000000b		; :
					DB		00001100b
					DB		00001100b
					DB		00000000b
					DB		00001100b
					DB		00001100b
					DB		00000000b
					DB		00000000b

					DB		6 * 8 DUP (0)	; ;<=>?@

					DB		26 * 8 DUP (0)	; A-Z

					DB		6 * 8 DUP (0)		; [\]^_'

					DB		00000000b		; a
					DB		00000000b
					DB		00011100b
					DB		00000010b
					DB		00011110b
					DB		00100010b
					DB		00011101b
					DB		00000000b

					DB		00000000b		; b
					DB		01100000b
					DB		00100000b
					DB		00111100b
					DB		00100010b
					DB		00100010b
					DB		01011100b
					DB		00000000b

					DB		00000000b		; c
					DB		00000000b
					DB		00011100b
					DB		00100010b
					DB		00100000b
					DB		00100010b
					DB		00011100b
					DB		00000000b

					DB		00000000b		; d
					DB		00000110b
					DB		00000010b
					DB		00011110b
					DB		00100010b
					DB		00100010b
					DB		00011101b
					DB		00000000b

					DB		00000000b		; e
					DB		00000000b
					DB		00011100b
					DB		00100010b
					DB		00111110b
					DB		00100000b
					DB		00011100b
					DB		00000000b

					DB		00000000b		; f
					DB		00011100b
					DB		00010010b
					DB		00010000b
					DB		00111100b
					DB		00010000b
					DB		00111000b
					DB		00000000b

					DB		00000000b		; g
					DB		00011101b
					DB		00100010b
					DB		00100010b
					DB		00011110b
					DB		00000010b
					DB		00111100b
					DB		00000000b

					DB		00000000b		; h
					DB		01100000b
					DB		00100000b
					DB		00101100b
					DB		00110010b
					DB		00100010b
					DB		01100010b
					DB		00000000b

					DB		00000000b		; i
					DB		00001000b
					DB		00000000b
					DB		00111000b
					DB		00001000b
					DB		00001000b
					DB		00111110b
					DB		00000000b

					DB		00000000b		; j
					DB		00001000b
					DB		00000000b
					DB		00111000b
					DB		00001000b
					DB		01001000b
					DB		00111000b
					DB		00000000b

					DB		00000000b		; k
					DB		01100000b
					DB		00100010b
					DB		00101100b
					DB		00110000b
					DB		00101000b
					DB		01100100b
					DB		00000000b

					DB		00000000b		; l
					DB		00111000b
					DB		00001000b
					DB		00001000b
					DB		00001000b
					DB		00001000b
					DB		00111110b
					DB		00000000b

					DB		00000000b		; m
					DB		00000000b
					DB		00111100b
					DB		00101010b
					DB		00101010b
					DB		00101010b
					DB		00100010b
					DB		00000000b

					DB		00000000b		; n
					DB		00000000b
					DB		00111100b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00000000b

					DB		00000000b		; o
					DB		00000000b
					DB		00011100b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00011100b
					DB		00000000b

					DB		00000000b		; p
					DB		01011100b
					DB		00100010b
					DB		00100010b
					DB		00111100b
					DB		00100000b
					DB		01110000b
					DB		00000000b

					DB		00000000b		; q
					DB		00111010b
					DB		01000100b
					DB		01000100b
					DB		00111100b
					DB		00000100b
					DB		00001110b
					DB		00000000b

					DB		00000000b		; r
					DB		01100000b
					DB		00101100b
					DB		00110110b
					DB		00100000b
					DB		00100000b
					DB		01110000b
					DB		00000000b

					DB		00000000b		; s
					DB		00011100b
					DB		00100010b
					DB		00011000b
					DB		00000100b
					DB		00100010b
					DB		00011100b
					DB		00000000b

					DB		00000000b		; t
					DB		00010000b
					DB		00111110b
					DB		00010000b
					DB		00010000b
					DB		00010010b
					DB		00001100b
					DB		00000000b

					DB		00000000b		; u
					DB		00000000b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00011101b
					DB		00000000b

					DB		00000000b		; v
					DB		00000000b
					DB		00100010b
					DB		00100010b
					DB		00100010b
					DB		00010100b
					DB		00001000b
					DB		00000000b

					DB		00000000b		; w
					DB		00000000b
					DB		00100010b
					DB		00100010b
					DB		00101010b
					DB		00010100b
					DB		00010100b
					DB		00000000b

					DB		00000000b		; x
					DB		00000000b
					DB		00100010b
					DB		00010100b
					DB		00001000b
					DB		00010100b
					DB		00100010b
					DB		00000000b

					DB		00000000b		; y
					DB		00010010b
					DB		00010010b
					DB		00010010b
					DB		00001100b
					DB		00001000b
					DB		00110000b
					DB		00000000b

					DB		00000000b		; z
					DB		00111110b
					DB		00100010b
					DB		00001100b
					DB		00110000b
					DB		00100010b
					DB		00111110b
					DB		00000000b

					end