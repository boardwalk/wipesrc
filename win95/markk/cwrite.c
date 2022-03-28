/* Write a block of data to outgoing serial buffer. */
#include	"c:\watcom\h\stdio.h"
#include	"c:\watcom\h\string.h"
#include	"..\markk\comio.h"
#include	"..\markk\comerr.h"
#include	"..\markk\debug.h"

long	cwrite(long device, char *buffer, long size)
{
	char	*buf = buffer;
	long	BufSize = size;
	long	SizeToEnd;
	long	WrapSize;
	
	fn_track("cwrite\n")

	/* Write Data from buffer to Transmit. */
	// NEED TO WATCH FOR WRAP when writing into circular buffer.
	if((com_TransmitTail + BufSize) > _TRANSMIT_BUFFER_SIZE) {
		// Wraparound write
		SizeToEnd = _TRANSMIT_BUFFER_SIZE - com_TransmitTail;
		memcpy(&com_TransmitBuffer[com_TransmitTail], buf, SizeToEnd);
		WrapSize = BufSize - SizeToEnd;
		memcpy(&com_TransmitBuffer[0], buf+SizeToEnd, WrapSize);
		com_TransmitTail = WrapSize;
	}
	else {
		memcpy(&com_TransmitBuffer[com_TransmitTail], buf, BufSize);
		com_TransmitTail += BufSize;
		if(com_TransmitTail == _TRANSMIT_BUFFER_SIZE) com_TransmitTail = 0;
	}

	/* Copy First Byte into THRE register. */
	/* and Trigger an interrupt sequence. */
//	com_TransmitData = com_TransmitBuffer[com_TransmitHead];
//	com_TransmitHead++;
//	if (com_TransmitHead > _TRANSMIT_BUFFER_SIZE) com_TransmitHead = 0;

	while(inp(com_PortSelect + _LINE_STATUS) & _TRANS_HOLDING == 0);
//	outp(com_PortSelect + _DATA, com_TransmitData);
	com_Register = inp(com_PortSelect + _INTERRUPT_ENABLE);
	while(com_Register & _ENABLE_TRANSMIT) com_Register = inp(com_PortSelect + _INTERRUPT_ENABLE);
	outp(com_PortSelect + _INTERRUPT_ENABLE, com_Register | _ENABLE_TRANSMIT);	// KICK START pmcom()
	
	#if	0
	#ifdef	FN_DEBUG
	memcpy(DebugString, buf, BufSize);
	DebugString[BufSize] = '\0';
	strcat(DebugString, "\n\0");
	fn_track(DebugString)
	#endif
	#endif
	
	return (OK);
}





















