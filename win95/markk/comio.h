//-----------------------------------------------------------------------//
// Serial low-level library header. 
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

/* Serial Device Part List. */
#define				_NO_UART				0
#define				_8250					1
#define				_8250A_16450			2
#define				_16C1450				3
#define				_16550A					4
#define				_16550AF_C_CF			5
#define				_16C1550				6
#define				_16552					7
#define				_82510					8

/* Device Constants. */
#define				_COM1					1
#define				_COM2					2
#define				_COM3					3
#define				_COM4					4

/* IRQ Constants. */
// COM1/3 == IRQ3; COM2/4 == IRQ4
#define				_IRQ3					3
#define				_IRQ4					4

/* Interrupt Constants. */
#define				_DOS_INT				0x21
#define				_MOUSE_INT				0x33

/* Serial Register Port List Table */
#define				_DATA					0
#define				_INTERRUPT_ENABLE		1

#define				_INTERRUPT_ID			2
#define				_LINE_CONTROL			3
#define				_MODEM_CONTROL			4
#define				_LINE_STATUS			5
#define				_MODEM_STATUS			6
#define				_SCRATCH_PAD			7

/* Baud Rate Divisor Ports. */
#define				_BAUD_RATE_DIVISOR_LSB	0
#define				_BAUD_RATE_DIVISOR_MSB	1

/* Port Values. */
#define				_END_OF_INTERRUPT		0x20

/* BIOS RAM Addresses. */
#define				_COM1_PORT				0x00000400
#define				_COM2_PORT				0x00000402
#define				_COM3_PORT				0x00000404
#define				_COM4_PORT				0x00000406

/* Programmable Interrupt Controller Values. */
#define				_PIC_CONTROL			0x0020
#define				_PIC_MASK				0x0021
#define				_READ_IN_SERVICE		0x0B
#define				_IRQ3_MASK				0x08

/* Baud-Rate Divisor Settings. */
#define				_110_BAUD				0x17
#define				_300_BAUD				0x80
#define				_1200_BAUD				0x60
#define				_2400_BAUD				0x30
#define				_4800_BAUD				0x18
#define				_9600_BAUD				0x0C
#define				_14400_BAUD				0x08
#define				_19200_BAUD				0x06
#define				_38400_BAUD				0x03
#define				_57600_BAUD				0x02
#define				_115200_BAUD			0x01

/* Interrupt-Enable Register Values. */
#define				_ENABLE_MODEM_CHANGE	0x08	// 00001000b
#define				_ENABLE_LINE_CHANGE		0x04	// 00000100b
#define				_ENABLE_TRANSMIT		0x02	// 00000010b
#define				_ENABLE_RECEIVE			0x01	// 00000001b
#define				_DISABLE_MODEM_CHANGE	0xF7	// 11110111b
#define				_DISABLE_LINE_CHANGE	0xFB	// 11111011b
#define				_DISABLE_TRANSMIT		0xFD	// 11111101b
#define				_DISABLE_RECEIVE		0xFE	// 11111110b

/* Interrupt-Identification Register Values. */
#define				_INTERRUPT_ID_MASK		0x06	// 00000110b
#define				_INTERRUPT_PENDING_MASK	0x01	// 00000001b

#define				_RECEIVE_LINE_STATUS	0x06	// 00000110b
#define				_RECEIVE_DATA_AVAILABLE	0x04	// 00000100b
#define				_TRANSMIT_HOLDING_EMPTY	0x02	// 00000010b
#define				_MODEM_STATUS_CHANGE	0x00	// 00000000b
#define				_INTERRUPT_NOT_PENDING	0x01	// 00000001b
#define				_INTERRUPT_PENDING		0x00	// 00000000b

/* Line-Control Register Values. */
#define				_SET_BAUD_RATE			0x80	// 10000000b
#define				_TRANSMIT_BREAK			0x40	// 01000000b
#define				_PARITY_NORMAL			0x20	// 00100000b
#define				_PARITY_EVEN			0x10	// 00010000b
#define				_PARITY_ENABLE			0x08	// 00001000b
#define				_STOP_BIT				0x04	// 00000100b

#define				_5BIT_DATA				0x00	// 00000000b
#define				_6BIT_DATA				0x01	// 00000001b
#define				_7BIT_DATA				0x02	// 00000010b
#define				_8BIT_DATA				0x03	// 00000011b
#define				_NO_PARITY				0
#define				_EVEN_PARITY			_PARITY_ENABLE ! _PARITY_EVEN
#define				_ODD_PARITY				_PARITY_ENABLE
#define				_1BIT_STOP				0
#define				_2BIT_STOP				_STOP_BIT

/* Modem-Control Register Values. */
#define				_LOOP_BACK_TEST			0x10	// 00010000b
#define				_ENABLE_8250			0x08	// 00001000b
#define				_DISABLE_8250			0x08	// 11110111b
#define				_MODEM_RESET			0x04	// 00000100b
#define				_RTS					0x02	// 00000010b
#define				_DTR					0x01	// 00000001b

/* Line-Status Register Values. */
#define				_TRANS_SHIFT			0x40	// 01000000b
#define				_TRANS_HOLDING			0x20	// 00100000b
#define				_BREAK_CONDITION		0x10	// 00010000b
#define				_FRAMING_ERROR			0x08	// 00001000b
#define				_PARITY_ERROR			0x04	// 00000100b
#define				_OVERRUN_ERROR			0x02	// 00000010b
#define				_RECEIVED_CHAR			0x01	// 00000001b

/* Modem-Status Register Values. */
#define				_RECEIVE_LINE			0x80	// 10000000b
#define				_RING_INDICATOR			0x40	// 01000000b
#define				_DSR					0x20	// 00100000b
#define				_CTS					0x10	// 00010000b
#define				_CHANGE_LINE			0x08	// 00001000b
#define				_CHANGE_RING			0x04	// 00000100b
#define				_CHANGE_DSR				0x02	// 00000010b
#define				_CHANGE_CTS				0x01	// 00000001b

/* Timeout Values. */
#define				_NO_TIMEOUT				0
#define				_THR_TIMEOUT			1
#define				_PACKET_TIMEOUT			2
#define				_BLOCK_TIMEOUT			3
#define				_CHUNK_TIMEOUT			4

/* Retry Values. */
#define				_THR_RETRIES			100

/* Size Constants. */
#define				_TRANSMIT_BUFFER_SIZE	4096
#define				_RECEIVE_BUFFER_SIZE	4096
#define				_FILLER_SIZE			32
#define				_CHUNK_SIZE				256
#define				_BLOCK_SIZE				16
#define				_PACKET_SIZE			3

/* Device Status. */
#define				_UNUSED					-1
#define				_MOUSE					0
#define				_SERIAL					1

/* Max values. */
#define				_MAX_SERIAL_DEVICES		4

/* Errors. */
#define				_COM_OK					0
#define				_COM_TIMEOUT_ERROR		-1
#define				_COM_CONNECT_ERROR		-2
#define				_COM_DEVICE_ERROR		-3
#define				_COM_CONFLICT_ERROR		-4

#define				_DEVICE_ON				1
#define				_DEVICE_OFF				0

//#define				_READ_TIMEOUT			4*(10000/24) // cread gr_info version
//#define				_READ_TIMEOUT			20*(10000/24) // cread gr_info version
#define				_READ_TIMEOUT			20*(10000/24) // cread gr_info version

//-----------------------------------------------------------------------//

extern volatile unsigned long	com_IRQSelect;
extern volatile unsigned long	com_PortSelect;

extern volatile unsigned char	com_ReceiveData;
extern volatile unsigned char	com_TransmitData;
extern volatile unsigned long	com_ReceiveHead;
extern volatile unsigned long 	com_ReceiveTail;
extern volatile unsigned long	com_TransmitHead;
extern volatile unsigned long	com_TransmitTail;

extern volatile unsigned char 	com_Register;
extern volatile unsigned char	com_InterruptPending;
extern volatile unsigned char	com_InterruptFlags;
extern volatile unsigned char	com_InterruptID;
extern volatile unsigned char	com_InterruptStatus;
extern volatile unsigned char	com_InServiceRegister;
extern volatile unsigned char	com_SerialSwitch;

extern volatile unsigned char	com_ReceiveBuffer[_RECEIVE_BUFFER_SIZE];
extern volatile unsigned char	com_TransmitBuffer[_TRANSMIT_BUFFER_SIZE];

//-----------------------------------------------------------------------//

extern long						copen(long device, long baud, long data, long parity, long stop);
extern long						cclose(long device);
extern long						cread(long device, char *buffer, long size);
extern long						cwrite(long device, char *buffer, long size);
extern long						getserialdevice(long port);
extern short					getserialport(long device);
extern long						installserialinterrupt(char intno);
extern long						removeserialinterrupt(char intno);
extern void __interrupt __far	pmcom();

//-----------------------------------------------------------------------//
