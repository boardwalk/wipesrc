//-------------------------------------------------------------------

// transfer function selection method
//#define		ORIGINAL_TRANSFER
//#define		TRANSFER_v1
//#define		TRANSFER_v2
//#define		TRANSFER_v3		// absolute packets with unused bytes dropped
//#define		TRANSFER_v4		// delta compression with absolute every 8th
//#define		TRANSFER_v5		// delta compression with cyclic absolute
//#define		TRANSFER_v6			// delta changes after 1st frame
#define		TRANSFER_v7			// delta changes after 1st frame and no_change compression

//#define		FN_DEBUG
//#define		FRAME_CHECKING

#define		CUT_DOWN_SIZE			4
#define		SHIP_SIZE				24
#define		DELTA_REDUCTION_SIZE	6

#define		MASTER_TRANSMIT_SIZE	(SHIP_SIZE - CUT_DOWN_SIZE) * (NO_OF_SHIPS - 1)
#define		MASTER_RECEIVE_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE)
#define		SLAVE_TRANSMIT_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE)
#define		SLAVE_RECEIVE_SIZE		(SHIP_SIZE - CUT_DOWN_SIZE) * (NO_OF_SHIPS - 1)

#define		MASTER_TRANSMIT_SIZE_DELTA	(SLAVE_TRANSMIT_SIZE) + (SHIP_SIZE - CUT_DOWN_SIZE - DELTA_REDUCTION_SIZE) * (NO_OF_SHIPS - 2)
#define		SLAVE_RECEIVE_SIZE_DELTA	MASTER_TRANSMIT_SIZE_DELTA

//#define		ABSOLUTE_SLAVE_PACKET	(SHIP_SIZE - CUT_DOWN_SIZE)
//#define		DELTA_MASTER_PACKET		MASTER_TRANSMIT_SIZE_DELTA

#define		MASTER_PACKET			SHIP_SIZE * (NO_OF_SHIPS - 1)
#define		ABSOLUTE_SLAVE_PACKET	SHIP_SIZE
#define		DELTA_SIZE				15
#define		DELTA_MASTER_PACKET		(DELTA_SIZE * (NO_OF_SHIPS - 1))
//#define 	DATA_PACKET_SIZE			168


//-------------------------------------------------------------------


#define 	WriteSIOData(b, l)		cwrite(CommSerialDevice, b, l)
#define 	ReadSIOData(b, l)		cread(CommSerialDevice, b, l)

#define		Long_2_3B(p, x)			*(p++) = (char) (x & 0x000000FF); \
									*(p++) = (char) ((x & 0x0000FF00) >> 8); \
									*(p++) = (char) ((x & 0x00FF0000) >> 16)

#define		Long_From_3B(p, x)		x = (long) (*(p++)); \
									x |= (long) (*(p++) << 8); \
									x |= (long) (*(p) << 16); \
									if((*(p++) & 0x80) != 0) x |= (long) (0xFF000000)

#define		Short_2_B(p, x)			*(p++) = (char) (x & 0x00FF)
#define		Short_2_UB(p, x)		*(p++) = (char) (x & 0x00FF)

#define		Short_From_B(p, x)		x = (short) (*(p)); \
									if((*(p++) & 0x80) != 0) x |= (0xFF00)
#define		Short_From_UB(p, x)		x = (short) (*(p++))




//-------------------------------------------------------------------

typedef struct	combatdata
{
	unsigned char	combatMode;
	NegconBuff		*controlData;
	unsigned char	status;
	unsigned char	exit;
	unsigned char	exitCount;
	void			(*combatControl)(struct combatdata *, struct ShipData *);
} combatData;

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

//-------------------------------------------------------------------

extern long			absoluteShip;
extern long			abs_Order[];
extern long			cycle_count;
extern long			exit_status_on;
extern long			delta_frames_on;

//-------------------------------------------------------------------

extern void			NewMaster(combatData *, ShipData *);
extern void			NewSlave(combatData *, ShipData *);

//-------------------------------------------------------------------
