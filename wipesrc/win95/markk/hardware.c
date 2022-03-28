/* Hardware interface routines for Serial Communication. */
#include	"c:\watcom\h\stdio.h"
#include	"c:\watcom\h\conio.h"
#include	"c:\watcom\h\dos.h"
#include	"..\markk\comio.h"

#define	_RATIONAL_SNP_CHECK

#pragma	off	(check_stack);
void __interrupt __far		pmcom()
{
#ifdef	_RATIONAL_SNP_CHECK
 	/* Check interrupt is not a Rational Systems Segment-Not-Present interrupt for IRQ3 (COM 2/4). */
	if(com_IRQSelect == _IRQ3) {
//		_disable();
		outp(_PIC_CONTROL, _READ_IN_SERVICE);
		com_InServiceRegister = inp(_PIC_CONTROL);
//		_enable();
		if((com_InServiceRegister & _IRQ3_MASK) == 0) return;
	}
#endif

	com_InterruptPending = 1;
	com_InterruptFlags = inp(com_PortSelect + _INTERRUPT_ID);
	while (com_InterruptPending)
	{
		com_InterruptID = com_InterruptFlags & _INTERRUPT_ID_MASK;
		switch (com_InterruptID)
		{
			case	_RECEIVE_LINE_STATUS	:	//com_Register = inp(com_PortSelect + _LINE_STATUS);
												break;
			case	_MODEM_STATUS_CHANGE	:	//com_Register = inp(com_PortSelect + _MODEM_STATUS);
												break;
			case	_RECEIVE_DATA_AVAILABLE	:	{
													com_ReceiveData = inp(com_PortSelect + _DATA);
													com_ReceiveBuffer[com_ReceiveTail] = com_ReceiveData;
													com_ReceiveTail++;
//													if(com_ReceiveTail > _RECEIVE_BUFFER_SIZE) com_ReceiveTail = 0;
													if(com_ReceiveTail == _RECEIVE_BUFFER_SIZE) com_ReceiveTail = 0;
												}									
												break;
			case	_TRANSMIT_HOLDING_EMPTY	:	{
													if (com_TransmitHead != com_TransmitTail) {
														com_TransmitData = com_TransmitBuffer[com_TransmitHead];
														com_TransmitHead++;
//														if(com_TransmitHead > _TRANSMIT_BUFFER_SIZE) com_TransmitHead = 0;
														if(com_TransmitHead == _TRANSMIT_BUFFER_SIZE) com_TransmitHead = 0;
														outp(com_PortSelect + _DATA, com_TransmitData);
													}
													else {
														com_InterruptStatus = inp(com_PortSelect + _INTERRUPT_ENABLE);
														outp(com_PortSelect + _INTERRUPT_ENABLE, com_InterruptStatus & _DISABLE_TRANSMIT);
													}
												}
												break;
			default							:	break;
		}

		com_InterruptFlags = inp(com_PortSelect + _INTERRUPT_ID);
		if (com_InterruptFlags & _INTERRUPT_PENDING_MASK) com_InterruptPending = 0;
	}
	com_InterruptStatus = inp(com_PortSelect + _INTERRUPT_ENABLE);
	outp(com_PortSelect + _INTERRUPT_ENABLE, 0);
	outp(com_PortSelect + _INTERRUPT_ENABLE, com_InterruptStatus);
	outp(_PIC_CONTROL, _END_OF_INTERRUPT);
}
#pragma	on	(check_stack);
