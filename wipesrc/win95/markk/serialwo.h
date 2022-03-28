//-----------------------------------------------------------------------//
// WIPEOUT PC Serial game routines header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

//#define				ORIGINAL_TRANSFER
//#define				TRANSFER_v1
//#define				TRANSFER_v2
//#define				TRANSFER_v3				// absolute packets with unused bytes dropped
//#define				TRANSFER_v4				// delta compression with absolute every 8th
//#define				TRANSFER_v5				// delta compression with cyclic absolute
//#define				TRANSFER_v6				// delta changes after 1st frame
#define					TRANSFER_v7				// delta changes after 1st frame and no_change compression

#define					CUT_DOWN_SIZE			4
#define					SHIP_SIZE				24
#define					DELTA_REDUCTION_SIZE	6
#define					MASTER_TRANSMIT_SIZE	(SHIP_SIZE - CUT_DOWN_SIZE) * (NO_OF_SHIPS - 1)
#define					MASTER_RECEIVE_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE)
#define					SLAVE_TRANSMIT_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE)
#define					SLAVE_RECEIVE_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE) * (NO_OF_SHIPS - 1)
#define					MASTER_TRANSMIT_SIZE_DELTA	(SLAVE_TRANSMIT_SIZE) + (SHIP_SIZE - CUT_DOWN_SIZE - DELTA_REDUCTION_SIZE) * (NO_OF_SHIPS - 2)
#define					SLAVE_RECEIVE_SIZE_DELTA	MASTER_TRANSMIT_SIZE_DELTA
//#define				ABSOLUTE_SLAVE_PACKET	(SHIP_SIZE - CUT_DOWN_SIZE)
//#define				DELTA_MASTER_PACKET		MASTER_TRANSMIT_SIZE_DELTA
#define					MASTER_PACKET			SHIP_SIZE * (NO_OF_SHIPS - 1)
#define					ABSOLUTE_SLAVE_PACKET	SHIP_SIZE
#define					DELTA_SIZE				15
#define					DELTA_MASTER_PACKET		(DELTA_SIZE * (NO_OF_SHIPS - 1))
//#define				 	DATA_PACKET_SIZE		168

#define 				SLAVE					1
#define					MASTER					0
#define					SINGLE					2
#define					MASTERPOLL				3
#define					SLAVEDETECT				4
#define					WAITING					0
#define					PROCESSING				1
#define					INIT					2
#define					EXIT_RETRY				30
#define					TIMEOUT					50000
#define 				BR						2073600
#define 				DATA_PACKET_SIZE		168
//#define					SLAVE_TRANSMIT_SIZE		24
//#define					MASTER_RECEIVE_SIZE		24

#define					COMBATDEBUG				0
#define					PAUSE					0x30	//190
#define					CONT					0x31	//200
#define					RETIRE					0x32	//210
#define					QUIT				    0x33	//220
#define					NULLDATA				0x34	//230

#define					CRC_ERROR				0x39

#define					SIOCM_STP0  			0x00000000		/* Stop bits */
#define					SIOCM_STP1  			0x00000040
#define					SIOCM_STP15 			0x00000080
#define					SIOCM_STP2  			0x000000C0
#define					SIOCM_PARO  			0x00000000		/* Parity type */
#define					SIOCM_PARE  			0x00000020
#define					SIOCM_NOPAR 			0x00000000		/* Parity Enable */
#define					SIOCM_PAR   			0x00000010
#define					SIOCM_DAT5  			0x00000000		/* Data length */
#define					SIOCM_DAT6  			0x00000004
#define					SIOCM_DAT7  			0x00000008
#define					SIOCM_DAT8  			0x0000000C
#define					SIOCL_DTRON 			0x00000001		/* Control lines */
#define					SIOCL_RTSON 			0x00000002

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

//-----------------------------------------------------------------------//

#define 				WriteSIOData(b, l)		cwrite(CommSerialDevice, b, l)
#define 				ReadSIOData(b, l)		cread(CommSerialDevice, b, l)
#define					Long_2_3B(p, x)			*(p++) = (char) (x & 0x000000FF); \
												*(p++) = (char) ((x & 0x0000FF00) >> 8); \
												*(p++) = (char) ((x & 0x00FF0000) >> 16)
#define					Long_From_3B(p, x)		x = (long) (*(p++)); \
												x |= (long) (*(p++) << 8); \
												x |= (long) (*(p) << 16); \
												if((*(p++) & 0x80) != 0) x |= (long) (0xFF000000)
#define					Short_2_B(p, x)			*(p++) = (char) (x & 0x00FF)
#define					Short_2_UB(p, x)		*(p++) = (char) (x & 0x00FF)
#define					Short_From_B(p, x)		x = (short) (*(p)); \
												if((*(p++) & 0x80) != 0) x |= (0xFF00)
#define					Short_From_UB(p, x)		x = (short) (*(p++))

//-----------------------------------------------------------------------//

#ifdef	_DEFINE_COMBAT_DATA_
typedef struct	combatdata
{
	unsigned char	combatMode;
	NegconBuff		*controlData;
	unsigned char	status;
	unsigned char	exit;
	unsigned char	exitCount;
	void			(*combatControl)(struct combatdata *, struct ShipData *);
} combatData;
#endif

typedef struct	_ship_packet
{
	long			vx;
	long			vy;
	long			vz;
	short			hdg;
	short			pitch;
	short			roll;
	short			haveFired;
	short			nearSecNo;
	short			prevSecNo;
} shipPacket;

typedef struct	_comp_packet
{
	signed short	vx;
	signed short	vy;
	signed short	vz;
	signed char 	hdg;
	signed char		pitch;
	signed char		roll;
	unsigned char	haveFired;
	signed char		nearSecNo;
	signed char		prevSecNo;
} compPacket;

//-----------------------------------------------------------------------//

extern long					CommLinkError;

extern long					CommSerialConnectMethod;
extern long					CommSerialMachine;
extern long					CommSerialDevice;
extern long					CommSerialBaudRate;
extern long					CommSerialDataSize;
extern long					CommSerialParity;
extern long					CommSerialStopSize;
extern char					CommMasterCount;
extern char					CommMasterCount2;
extern long					CommMultiPlaySelect;

extern long					cycle_count;
extern long					exit_status_on;
extern long					delta_frames_on;

//-----------------------------------------------------------------------//

extern long					MAK_SerialSynchronize(void);
extern long					MAK_CheckSerialHandshake(void);
extern long					MAK_SendSerialSync(void);
extern long					MAK_SendSerialAckn(void);
extern long					MAK_SearchSerialSync(void);
extern long					MAK_SearchSerialAckn(void);
extern long					MAK_ResolveSerial(void);
extern long					MAK_ResolveSerialMasterSlave(void);
#ifdef	_DEFINE_COMBAT_DATA_
extern void					NewMaster(combatData *packetData, ShipData *shipInfo);
extern void					NewSlave(combatData *packetData, ShipData *shipInfo);
#endif

//-----------------------------------------------------------------------//
