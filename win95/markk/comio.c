//-----------------------------------------------------------------------//
// Serial low-level library.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

#include			"c:\wc10\h\dos.h"
#include			"c:\wc10\h\stdio.h"
#include			"..\markk\comio.h"
#include			"..\markk\merror.h"
#include			"..\markk\debug.h"

//-----------------------------------------------------------------------//

/* Register Interface. */
volatile union REGS				com_regs;
volatile struct SREGS		 	com_sregs;

/* Hardware Configuration Variables. */
volatile unsigned long			com_DeviceSelect;
volatile unsigned long			com_ChipSelect;
volatile unsigned long			com_IRQSelect;
volatile unsigned long			com_PortSelect;
volatile unsigned long			com_BaudSelect;
volatile unsigned long			com_DataSelect;
volatile unsigned long			com_ParitySelect;
volatile unsigned long			com_StopSelect;
volatile unsigned long			com_OriginalIntOffset;
volatile unsigned short			com_OriginalIntSelector;

/* Interrupt Critical Variables. */
volatile unsigned char			com_ReceiveData;
volatile unsigned char			com_TransmitData;
volatile unsigned long			com_ReceiveHead;
volatile unsigned long			com_ReceiveTail;
volatile unsigned long			com_TransmitHead;
volatile unsigned long			com_TransmitTail;

volatile unsigned char			com_Register;
volatile unsigned char			com_InterruptPending;
volatile unsigned char			com_InterruptFlags;
volatile unsigned char			com_InterruptID;
volatile unsigned char			com_InterruptStatus;
volatile unsigned char			com_InServiceRegister;
volatile unsigned char			com_SerialSwitch;
volatile unsigned char			com_notused;

volatile unsigned char			com_ReceiveBuffer[_RECEIVE_BUFFER_SIZE];
volatile unsigned char			com_Filler1[_FILLER_SIZE];
volatile unsigned char			com_TransmitBuffer[_TRANSMIT_BUFFER_SIZE];
volatile unsigned char			com_Filler2[_FILLER_SIZE];

/* Main Variables. */

//-----------------------------------------------------------------------//

/* Open serial device. */
long				copen(long device, long baud, long data, long parity, long stop)
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
#ifdef PORT_MOUSE
	int386(_MOUSE_INT, &com_regs, &com_regs);
#endif
	if(com_regs.h.cl == com_IRQSelect) return (_ERR_INSTALLING_SERIAL);

	/* Autodetect available serial chips and port addresses. */
	com_PortSelect = getserialport(com_DeviceSelect);
	if(!com_PortSelect) return (_ERR_INSTALLING_SERIAL);
	com_ChipSelect = getserialdevice(com_PortSelect);

	/* Clear Circular Buffers. */
	memset(com_ReceiveBuffer, 0, _RECEIVE_BUFFER_SIZE);
	memset(com_TransmitBuffer, 0, _TRANSMIT_BUFFER_SIZE);
	com_ReceiveHead = 0;
	com_ReceiveTail = 0;
	com_TransmitHead = 0;
	com_TransmitTail = 0;
		
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
	installserialinterrupt(com_IRQSelect + 8);

	/* Allow 8259 IRQ interrupt. */
	com_Register = inp(_PIC_MASK);
	outp(_PIC_MASK, com_Register & ~(1 << com_IRQSelect));
	
	/* Turn interrupts back on. */
	_enable();

	/* serial device opened successfully. */
	return (OK);
}

//-----------------------------------------------------------------------//

/* Close serial device. */
long				cclose(long device)
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
		case	_COM1	:	removeserialinterrupt(_IRQ4 + 8);
							break;
		case	_COM2	:	removeserialinterrupt(_IRQ3 + 8);
							break;
		case	_COM3	:	removeserialinterrupt(_IRQ4 + 8);
							break;
		case	_COM4	:	removeserialinterrupt(_IRQ3 + 8);
							break;
		default			:	return (_ERR_BAD_SERIAL_PORT);
							break;							
	}

	/* Serial device closed successfully. */
	return (OK);
}

//-----------------------------------------------------------------------//

/* copy data from receive serial buffer. */
long				cread(long device, char *buffer, long size)
{
	char	*buf = buffer;
	long	BufSize = size;
	long	SizeToEnd;
	long	WrapSize;
	long	time = 0;
	long	not_ready = 1;

	fn_track("cread\n")
		
	/* Wait until at least enough data has arrived. */
	while(not_ready) {
		if((com_ReceiveTail > com_ReceiveHead) && (com_ReceiveTail - com_ReceiveHead >= BufSize)) break;
		if((com_ReceiveTail < com_ReceiveHead) && ((_RECEIVE_BUFFER_SIZE - com_ReceiveHead + com_ReceiveTail) >= BufSize)) break;
		time++;
		if(time > _READ_TIMEOUT) return (_ERR_TIMEOUT_SERIAL);
		
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
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Write data out to transmit buffer and start transmitting over serial line. */
long				cwrite(long device, char *buffer, long size)
{
	char	*buf = buffer;
	long	BufSize = size;
	long	SizeToEnd;
	long	WrapSize;
	
	fn_track("cwrite\n")

	/* Write Data from buffer to Transmit. */
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

	// trigger the background interrupt
	while(inp(com_PortSelect + _LINE_STATUS) & _TRANS_HOLDING == 0);
	com_Register = inp(com_PortSelect + _INTERRUPT_ENABLE);
	while(com_Register & _ENABLE_TRANSMIT) com_Register = inp(com_PortSelect + _INTERRUPT_ENABLE);
	outp(com_PortSelect + _INTERRUPT_ENABLE, com_Register | _ENABLE_TRANSMIT);	// KICK START pmcom()
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Autodetect serial chip. */
long				getserialdevice(long port)
{
	/* send at least 8250 chip. */
	return (_8250);
}

//-----------------------------------------------------------------------//

/* Autodetect device's port address, using BIOS RAM area. */
short				getserialport(long device)
{
	short	*port;

	/* locate port address given device type. */
	switch (device)
	{
		case	_COM1	:	port = (short *) _COM1_PORT;
							break;
		case	_COM2	:	port = (short *) _COM2_PORT;
							break;
		case	_COM3	:	port = (short *) _COM3_PORT;
							break;
		case	_COM4	:	port = (short *) _COM4_PORT;
							break;
		default			:	return (_ERR_BAD_SERIAL_PORT);
	}

	/* return result. */
	if(*port) return (*port);
	else return (_ERR_SERIAL_PORT_NOT_FOUND);
}

//-----------------------------------------------------------------------//

/* Save original serial handler and hook new serial interrupt handler. */
long				installserialinterrupt(char intno)
{
	/* Save Original PM Serial Int. */
	com_regs.x.eax = 0x00003500;
	com_regs.h.al = intno;
#ifdef PORT_SERIAL
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
#endif
	com_OriginalIntSelector = com_sregs.es;
	com_OriginalIntOffset = com_regs.x.ebx;
	
	/* Install New LIBCOMM Serial Int. */
	com_regs.x.eax = 0x00002500;
	com_regs.h.al = intno;
	com_sregs.ds = FP_SEG((void __far *) pmcom);
	com_regs.x.edx = FP_OFF((void __far *) pmcom);
#ifdef PORT_SERIAL
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
#endif
	
	return (OK);
}

//-----------------------------------------------------------------------//

/* Dehook serial interrupt handler and restore old handler. */
long				removeserialinterrupt(char intno)
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
#ifdef PORT_SERIAL
	int386x(_DOS_INT, &com_regs, &com_regs, &com_sregs);
#endif
	
	return (OK);
}

//-----------------------------------------------------------------------//
