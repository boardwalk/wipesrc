// fully tested version
/* Open communication on an asynchronous serial device. */
#include	"c:\watcom\h\stdio.h"
#include	"c:\watcom\h\dos.h"
#include	"c:\watcom\h\string.h"
#include	"..\markk\comio.h"
#include	"..\markk\comerr.h"

long	copen(long device, long baud, long data, long parity, long stop)
{
	/* Save parameters. */
	com_DeviceSelect = device;
	com_BaudSelect = baud;
	com_DataSelect = data;
	com_ParitySelect = parity;
	com_StopSelect = stop;
	
	/* Assign IRQ Setting, given device. */
	switch (com_DeviceSelect)  {
		case	_COM1	:	com_IRQSelect = _IRQ4;
							break;
		case	_COM2	:	com_IRQSelect = _IRQ3;
							break;
		case	_COM3	:	com_IRQSelect = _IRQ4;
							break;
		case	_COM4	:	com_IRQSelect = _IRQ3;
							break;
		default			:	return _COM_DEVICE_ERROR;
							break;
	}

	/* Check serial device not in use by a serial mouse. */
	com_regs.w.ax = 0x0024;
	int386(_MOUSE_INT, &com_regs, &com_regs);
	if(com_regs.h.cl == com_IRQSelect) return (_ERR_INSTALLING);

	/* Autodetect available serial chips and port addresses. */
	com_PortSelect = autoport(com_DeviceSelect);
	if(!com_PortSelect) return (_ERR_INSTALLING);
	com_ChipSelect = autocom(com_PortSelect);

	/* Clear Circular Buffers. */
	memset(com_ReceiveBuffer, 0, _RECEIVE_BUFFER_SIZE);
	memset(com_TransmitBuffer, 0, _TRANSMIT_BUFFER_SIZE);
	
	/* DisAllow 8259 IRQ interrupt. */
	com_Register = inp(_PIC_MASK);
	outp(_PIC_MASK, com_Register | (1 << com_IRQSelect));

	/* Shut off interrupts. */
	_disable();

	/* Set Modem Control Register. */
	com_Register = inp(com_PortSelect + _MODEM_CONTROL);
	outp(com_PortSelect + _MODEM_CONTROL, _ENABLE_8250 | com_Register); // | _RTS | _DTR);

	/* Set Line Control Register. */
	outp(com_PortSelect + _LINE_CONTROL, _SET_BAUD_RATE | com_DataSelect | com_ParitySelect | com_StopSelect);
	outp(com_PortSelect + _BAUD_RATE_DIVISOR_LSB, com_BaudSelect);
	outp(com_PortSelect + _BAUD_RATE_DIVISOR_MSB, 0);
	outp(com_PortSelect + _LINE_CONTROL, com_DataSelect | com_ParitySelect | com_StopSelect);

	/* Set Interrupt Enable Register. */
//	outp(com_PortSelect + _INTERRUPT_ENABLE, _ENABLE_RECEIVE | _ENABLE_MODEM_CHANGE | _ENABLE_LINE_CHANGE);
	outp(com_PortSelect + _INTERRUPT_ENABLE, _ENABLE_RECEIVE);

	/* Install New Serial ISR Routine. */
	InstallSerialPMInterrupt(com_IRQSelect + 8);

	/* Allow 8259 IRQ interrupt. */
	com_Register = inp(_PIC_MASK);
	outp(_PIC_MASK, com_Register & ~(1 << com_IRQSelect));
	
	/* Turn interrupts back on. */
	_enable();

	/* serial device opened successfully. */
	return (OK);
}


void	InstallSerialPMInterrupt(char intno)
{
	/* Save Original PM Serial Int. */
	com_regs.x.eax = 0x00003500;
	com_regs.h.al = intno;
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
	com_OriginalIntSelector = com_sregs.es;
	com_OriginalIntOffset = com_regs.x.ebx;
	
	/* Init Circular Buffers. */
	com_ReceiveHead = 0;
	com_ReceiveTail = 0;
	com_TransmitHead = 0;
	com_TransmitTail = 0;
//	com_SendCount = 0;
//	com_ReceiveCount = 0;
	
	/* Install New LIBCOMM Serial Int. */
	com_regs.x.eax = 0x00002500;
	com_regs.h.al = intno;
	com_sregs.ds = FP_SEG((void __far *) pmcom);
	com_regs.x.edx = FP_OFF((void __far *) pmcom);
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
}
