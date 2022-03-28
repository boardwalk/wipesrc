extern int VSync(int mode);
#pragma aux VSync=							\
				"push	dx"					\
   				"push	ax"					\
"vloop001:	 	mov	dx,03dah"				\
				"in	al,dx"					\
				"test	al,1000b"			\
				"jnz	vloop001"			\
"vloop002:	 	mov	dx,03dah"				\
				"in	al,dx"					\
				"test	al,1000b"			\
				"jz	vloop002"				\
				"pop	ax"					\
				"pop	dx"					\
				parm	[eax]				\
				value	[eax]				\
				modify [dx ax];


