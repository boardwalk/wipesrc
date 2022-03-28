/* Open a communication interface over a serial connection. */
#include	"..\markk\comio.h"
#include	"..\markk\comerr.h"

long	cclose(long device)
{
	/* Shut off 8259 IRQ Interrupt. */
	com_Register = inp(_PIC_MASK);
	outp(_PIC_MASK, com_Register | (1 << com_IRQSelect));
	
	/* Disable 8250 Interrupt Enable Register. */
	outp(com_PortSelect + _INTERRUPT_ENABLE, 0);
	
	/* Disable Modem Control Register. */
	com_Register = inp(com_PortSelect + _MODEM_CONTROL);
	outp(com_PortSelect + _MODEM_CONTROL, com_Register & _DISABLE_8250);

	/* Restore old ISR routine. */
	switch (device)  {
		case	_COM1	:	RemoveSerialPMInterrupt(_IRQ4 + 8);
							break;
		case	_COM2	:	RemoveSerialPMInterrupt(_IRQ3 + 8);
							break;
		case	_COM3	:	RemoveSerialPMInterrupt(_IRQ4 + 8);
							break;
		case	_COM4	:	RemoveSerialPMInterrupt(_IRQ3 + 8);
							break;
		default			:	return _COM_DEVICE_ERROR;
							break;							
	}

	/* Serial device closed successfully. */
	return (OK);
}


void	RemoveSerialPMInterrupt(char intno)
{
	/* Init Circular Buffers. */
	com_ReceiveHead = 0;
	com_ReceiveTail = 0;
	com_TransmitHead = 0;
	com_TransmitTail = 0;

	/* Setup Interrupt Enable Register. */
	outp(com_PortSelect + _INTERRUPT_ENABLE, 0);
	outp(com_PortSelect + _INTERRUPT_ENABLE, 0);
	
	/* Install Original PM Serial Int. */
	com_regs.x.eax = 0x00002500;
	com_regs.h.al = intno;
	com_sregs.ds = com_OriginalIntSelector;
	com_regs.x.edx= com_OriginalIntOffset;
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
}
