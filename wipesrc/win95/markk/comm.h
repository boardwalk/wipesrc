//-------------------------------------------------------------------
// ridge racer serial module
// this code now works with new Ridge Racer code (17/08/95)
//-------------------------------------------------------------------

/* Multiplayer Code Switches */
#define					MAK_LEAD
//#define					MAK_FOLLOW
#define					TEST_SERIAL
//#define				TEST_NETWORK
#define					_MULTI_PLAY
//#define				MAK_LOCK_TEST
#define					UNLIMITED_TIME
#define					CAR_COLLISIONS_ON
//#define					MAK_MULTI_PLAYER_TESTING
#define					SOUND_OFF
//#define				NO_TIME_OVER
#define					SERIAL_READ_DELAY
#define					CAR_POSITIONS_ON
//#define					FILE_DATA_ANALYSIS
//#define				SERIAL_DEBUGGING
#define					FN_DEBUG
//#define					TEST_MODEM

//-------------------------------------------------------------------

#define					M_CAR_MAX				C_CAR_MAX + 1
#define					_MAX_NUM_CARS			12
#define					_MAX_LINK_CARS			13
#define					_MAX_TRANSMIT_CARS		13
#define					_SINGLE_PLAY			1
#define					_HEAD_2_HEAD			2
#define					_TCP_NETWORK			3
#define					_LEAD					1
#define					_FOLLOW					2

#define					_DIALUP					1
#define					_DIRECT					2

#define					_SYNC_TIMEOUT			1000
#define					_SYNC_PACKET_SIZE		4
#define					_PAD_DATA_SIZE			4
#define					_HANDSHAKE_RETRIES		24*10

#define					_1_FRAME				1000/24

//-------------------------------------------------------------------

extern void				MAK_InitMulti(void);
extern void				MAK_OpenLink(void);
extern void				MAK_CloseLink(void);
extern long				MAK_OpenDevice(void);
extern void				MAK_CloseDevice(void);
extern void 			MAK_ReadFrame(void);
extern void 			MAK_WriteFrame(void);
extern long				MAK_SerialHandshake(void);
extern void				MAK_SetupMultiGame(void);
extern long				MAK_CheckHandshake(void);
extern void				MAK_ReportConnectFailure(long error_code);
extern void				MAK_SendSync(void);
extern void 			MAK_SendAckn(void);
extern long				MAK_SearchSync(void);
extern long				MAK_SearchAckn(void);
extern void				MAK_ProcessLinkPads(void);
extern void				MAK_ProcessThisPad(void);
extern void				MAK_ReadPadInfo(long padno);
extern void				MAK_CopyPadBuf(long padno);

extern void				MAK_SetupMultiData(void);
extern void				MAK_SetupMultiCars(void);

extern void				MAK_InitCarData(void);
extern void				MAK_ClearCar(CAR *c);
extern void				MAK_InitCommData(void);

//-------------------------------------------------------------------
extern void				SerialDataReport(char *data, long n);
