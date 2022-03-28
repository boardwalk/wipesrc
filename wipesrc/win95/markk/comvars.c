#include				"c:\watcom\h\dos.h"
#include				"..\markk\comio.h"

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
volatile unsigned short			com_OriginalIntSelector;
volatile unsigned long			com_OriginalIntOffset;

/* Interrupt Critical Variables. */
volatile unsigned char			com_InterruptPending;
volatile unsigned char			com_InterruptFlags;
volatile unsigned char			com_InterruptID;
volatile unsigned char			com_InterruptStatus;
volatile unsigned char			com_InServiceRegister;
volatile unsigned char			com_SerialSwitch;

volatile unsigned char			com_ReceiveData;
volatile unsigned char			com_TransmitData;
volatile unsigned long			com_ReceiveHead = 0;
volatile unsigned long			com_ReceiveTail = 0;
volatile unsigned long			com_TransmitHead = 0;
volatile unsigned long			com_TransmitTail = 0;

volatile unsigned char			com_ReceiveBuffer[_RECEIVE_BUFFER_SIZE];
volatile unsigned char			com_Filler1[_FILLER_SIZE];
volatile unsigned char			com_TransmitBuffer[_TRANSMIT_BUFFER_SIZE];
volatile unsigned char			com_Filler2[_FILLER_SIZE];

//extern unsigned char	com_TransmitEnd = 0;

/* Main Variables. */
volatile unsigned char			com_Register;

volatile long					com_IntCount = 0;			// for debugging
volatile long					com_SendCount = 0;			// for debugging
volatile long					com_ReceiveCount = 0;		// for debugging

