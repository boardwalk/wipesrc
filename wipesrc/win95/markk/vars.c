#include				<dos.h>
#include				"comio.h"

/* Register Interface. */
union REGS				com_regs;
struct SREGS		 	com_sregs;

/* Hardware Configuration Variables. */
char					com_DeviceSelect;
char					com_ChipSelect;
char					com_IRQSelect;
short					com_PortSelect;
char					com_BaudSelect;
char					com_DataSelect;
char					com_ParitySelect;
char					com_StopSelect;
short					*com_Port;
unsigned short			com_OriginalIntSelector;
unsigned long			com_OriginalIntOffset;

/* Interrupt Critical Variables. */
unsigned char			com_InterruptPending;
unsigned char			com_InterruptFlags;
unsigned char			com_InterruptID;
unsigned char			com_InterruptStatus;
unsigned char			com_InServiceRegister;

unsigned char			com_ReceiveData;
unsigned char			com_ReceiveBuffer[_RECEIVE_BUFFER_SIZE];
unsigned long			com_ReceiveHead = 0;
unsigned long			com_ReceiveTail = 0;

unsigned char			com_TransmitData;
unsigned char			com_TransmitBuffer[_TRANSMIT_BUFFER_SIZE];
unsigned long			com_TransmitHead = 0;
unsigned long			com_TransmitTail = 0;

extern unsigned char	com_TransmitEnd = 0;

/* Main Variables. */
unsigned char			com_Register;

long					com_IntCount = 0;			// for debugging
long					com_SendCount = 0;			// for debugging
long					com_ReceiveCount = 0;		// for debugging

