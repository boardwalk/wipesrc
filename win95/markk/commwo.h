//-------------------------------------------------------------------
/* Multiplayer Code Switches */

#define					MAK_LEAD
//#define					MAK_FOLLOW
#define					TEST_SERIAL
//#define				TEST_NETWORK
#define					_MULTI_PLAY
//#define					SOUND_OFF
//#define				NO_TIME_OVER
#define					SERIAL_READ_DELAY
//#define					TEST_MODEM

//-------------------------------------------------------------------

#define					TRUE					1
#define					FALSE					0
#define					_SINGLE_PLAY			1
#define					_HEAD_2_HEAD			2
#define					_TCP_NETWORK			3
#define					_LEAD					1
#define					_FOLLOW					2
#define					_DIALUP					1
#define					_DIRECT					2
#define					_SYNC_PACKET_SIZE		4
#define					_HANDSHAKE_RETRIES		30*10
#define					_1_FRAME				1000/24

//-------------------------------------------------------------------

extern long				CommMultiPlaySelect;
extern long				CommNetPlayers;
extern long				CommSerialConnectMethod;
extern long				CommSerialMachine;
extern long				CommSerialDevice;
extern long				CommSerialBaudRate;
extern long				CommSerialDataSize;
extern long				CommSerialParity;
extern long				CommSerialStopSize;
extern long				CommModemMachine;
extern char				CommModemInitString[256];
extern char				CommModemSetupString[256];
extern char				CommModemDialupString[256];
extern char				CommModemAnswerString[256];
extern char				CommModemHangupString[256];
extern long				CommFirstRead;
extern long				CommLinkCompleted;
extern long				CommLinkError;
extern char				CommMasterCount;
extern char				CommMasterCount2;

//-------------------------------------------------------------------

extern void				MAK_OpenLink(void);
extern void				MAK_CloseLink(void);
extern long				MAK_OpenDevice(void);
extern void				MAK_CloseDevice(void);

extern long				MAK_SerialHandshake(void);
extern long				MAK_CheckHandshake(void);
extern void				MAK_ReportConnectFailure(long error_code);
extern void				MAK_SendSync(void);
extern void 			MAK_SendAckn(void);
extern long				MAK_SearchSync(void);
extern long				MAK_SearchAckn(void);
extern void				MAK_SelectMasterSlave(void);
extern long				MAK_ResolveMasterSlave(void);

//-------------------------------------------------------------------
