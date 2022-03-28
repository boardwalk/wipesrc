;		public	PUSH_ALL_
;		public	POP_ALL_
		public	_fpstate

.386P

dgroup	group _data


		





_data		segment para public use32 'data'
		assume ds:_data

_fpstate	db	108 dup(?)

_data		ends








_text		segment para public use32 'code'
		assume	cs:_text

if 0
PUSH_ALL_:
		pushad
		pushfd
		fsave	[fpstate]
		ret

POP_ALL_:
		frstor	[fpstate]
		popfd
		popad
		ret
endif
		
_text		ends
 		end
