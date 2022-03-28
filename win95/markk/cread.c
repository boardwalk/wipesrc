// fully tested version

/* Read a block of data from incoming serial buffer. */
#include	"c:\watcom\h\dos.h"
#include	"c:\watcom\h\stdlib.h"
#include	"c:\watcom\h\stdio.h"
#include	"..\markk\comio.h"
#include	"..\markk\comerr.h"
#include	"..\markk\debug.h"

long	LOCK_COUNT;
long	CREAD_COUNT = 0;
char	String[256];

long	cread(long device, char *buffer, long size)
{
//	char	modem_status = 0;
//	char	line_status = 0;

	char	*buf = buffer;
	long	BufSize = size;
	long	SizeToEnd;
	long	WrapSize;
	long	time = 0;
	long	not_ready = 1;

	fn_track("cread\n")
	
	#ifdef	_GR_READ_LOCK_ON
	LOCK_COUNT = 0;
	#endif

	#if	0
	modem_status = inp(com_PortSelect + _MODEM_STATUS);	
	outp(com_PortSelect + _MODEM_STATUS, modem_status | _DTR);
	delay(2);
	modem_status = inp(com_PortSelect + _MODEM_STATUS);
	if(!(modem_status & _DSR)) return (_ERR_TIMEOUT);
	line_status = inp(com_PortSelect + _LINE_STATUS);
	if(!(line_status & _RECEIVED_CHAR)) return (_ERR_TIMEOUT);
	#endif
		
	/* Wait until at least enough data has arrived. */
	while(not_ready) {
		if((com_ReceiveTail > com_ReceiveHead) && (com_ReceiveTail - com_ReceiveHead >= BufSize)) break;
		if((com_ReceiveTail < com_ReceiveHead) && ((_RECEIVE_BUFFER_SIZE - com_ReceiveHead + com_ReceiveTail) >= BufSize)) break;
		time++;
		if(time > _READ_TIMEOUT) return (_ERR_TIMEOUT);
		
//		delay(10);
//		delay(5);
		delay(1);
		
		// try smaller??
		
		#ifdef	_GR_READ_LOCK_ON
		LOCK_COUNT++;
//		WriteVGAString(0, _READ_LOCK, "read lok: ");
//		ltoa(LOCK_COUNT, String, 16);
//		WriteVGAString(10, _READ_LOCK, String);

		WriteVGAString(0, 20*8, "read lock: "); \
		fn_file = fopen("FNTRACK", "at"); \
		fprintf(fn_file, "read lock: "); \
		fclose(fn_file);

		ultoa(LOCK_COUNT, DebugString, 16); \
		strcat(DebugString, "\n"); \
		fn_file = fopen("FNTRACK", "at"); \
		fprintf(fn_file, DebugString); \
		fclose(fn_file);

		#endif					   
		#ifdef	_GR_RECEIVE_PTRS_ON
		WriteVGAString(0, _RECEIVE_HEAD_PTR, "rec head:");
		ultoa(com_ReceiveHead, String, 16);
		WriteVGAString(10, _RECEIVE_HEAD_PTR, String);
		WriteVGAString(0, _RECEIVE_TAIL_PTR, "rec tail:");
		ultoa(com_ReceiveTail, String, 16);
		WriteVGAString(10, _RECEIVE_TAIL_PTR, String);
		#endif
	}

	#ifdef	_GR_READ_COUNTER_ON
	CREAD_COUNT++;
	WriteVGAString(0, _READ_COUNTER, "read cnt: ");
	ltoa(CREAD_COUNT, String, 16);
	WriteVGAString(10, _READ_COUNTER, String);
	#endif

	/* Read data into buffer. */
	// WATCH FOR BUFFER WRAP!!!
	if((com_ReceiveHead + BufSize) > _RECEIVE_BUFFER_SIZE) {
		// Wraparound read
		SizeToEnd = _RECEIVE_BUFFER_SIZE - com_ReceiveHead;
		memcpy(buf, &com_ReceiveBuffer[com_ReceiveHead], SizeToEnd);
		WrapSize = BufSize - SizeToEnd;
		memcpy(buf+SizeToEnd, &com_ReceiveBuffer, WrapSize);
		com_ReceiveHead = WrapSize;
	}
	else {
		memcpy(buf, &com_ReceiveBuffer[com_ReceiveHead], BufSize);
		com_ReceiveHead += BufSize;
		if(com_ReceiveHead == _RECEIVE_BUFFER_SIZE) com_ReceiveHead = 0;
	}
	
	#if	0
	#ifdef	FN_DEBUG
	memcpy(DebugString, buf, 4);
	DebugString[4] = '\0';
	strcat(DebugString, "\n\0");
	fn_track(DebugString)
	#endif
	#endif
	
	return (OK);
}	



	