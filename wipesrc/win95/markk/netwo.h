//-----------------------------------------------------------------------//
// WIPEOUT PC Network game routines header.
// Copyright(C). Sony Interactive. (05/09/95). Mark Kelly.
//-----------------------------------------------------------------------//

//#define					FN_DEBUG

#define					ID_SIZE							4
#define					COMMAND_SIZE					4
#define					ID_PACKET_SIZE					sizeof(struct _id_packet)

#if	1
#define					HANDSHAKE_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _resolve_packet)
#define					RESOLVE_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _resolve_packet)
#define					SYNC_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _resolve_packet)
#define					MASTER_MENU_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					SLAVE_MENU_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _slave_menu_packet)
#define					PLAY_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _resolve_packet)
#endif

#if	0
#define					HANDSHAKE_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					RESOLVE_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					SYNC_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					MASTER_MENU_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					SLAVE_MENU_PACKET_SIZE			ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#define					PLAY_PACKET_SIZE				ID_PACKET_SIZE + sizeof(struct _master_menu_packet)
#endif

#define					MASTER_RACE_PACKET_SIZE			ID_PACKET_SIZE + (sizeof(struct _race_packet) * (MAX_NET_MACHINES))
#define					SLAVE_RACE_PACKET_SIZE			ID_PACKET_SIZE + (sizeof(struct _race_packet))

#define					HANDSHAKE_PACKET				0x10
#define					RESOLVE_PACKET					0x20
#define					SYNC_PACKET						0x30
#define					ACKN_PACKET						0x40
#define					MENU_PACKET						0x50
#define					PLAY_PACKET						0x60
#define					RACE_PACKET						0x70

#define					NO_OF_SHIPS						8
#define					MAX_NET_MACHINES				8
#define					_NETWORK_HANDSHAKE_RETRIES		50*10
#define					_NETWORK_RESOLVE_RETRIES		50*10

#define					MASTER							0
#define					SLAVE							1

#define					PAUSE							0x30	//190 
#define					CONT							0x31	//200 
#define					RETIRE							0x32	//210 
#define					QUIT							0x33	//220 
#define					NULLDATA						0x34	//230 

#define					SLAVE1							1
#define					SLAVE2							2
#define					SLAVE3							3
#define					SLAVE4							4
#define					SLAVE5							5
#define					SLAVE6							6
#define					SLAVE7							7
#define					NSINGLE							10

//-----------------------------------------------------------------------//

typedef struct _id_packet {
	unsigned long		PktSize;
	unsigned char		GameID[ID_SIZE];
	unsigned long		PktType;
	unsigned long		PktSyncCounter;
} ID_PKT;

typedef struct _resolve_packet {
	unsigned char		command[COMMAND_SIZE];
	unsigned char		network[NETWORK_ADDRESS_SIZE];
	unsigned char		node[NODE_ADDRESS_SIZE];
	unsigned short		activity;
} RES_PKT;

typedef struct _slave_menu_packet {
	unsigned long		SlaveShipNum;
	unsigned long		SlaveTeam;
	unsigned long		SlaveExitMenu;
} SLAVE_MENU_PKT;

typedef struct _master_menu_packet {
	unsigned long		Class;
	unsigned long		TrackNum;
	unsigned long		ActTrackNum;
	unsigned long		ShipNum[MAX_NET_PLAYERS];
	unsigned long		Team[MAX_NET_PLAYERS];
	unsigned long		ExitMenu[MAX_NET_PLAYERS];
} MASTER_MENU_PKT;

typedef struct _race_packet {
	long				MachineID;
	short				haveFired;
	short				hdg;
	short				pitch;
	short				roll;
	short				nearSecNo;
	short				prevSecNo;
	long				vx;
	long				vy;
	long				vz;
} RACE_PKT;

#ifdef	_DEFINE_CONFIG_DATA_
typedef struct ConfigData
{
	int				defaultMenu;
	int				masterWins;
	int				slaveWins;
    int     		modelHdg;

	short			gameOverCount;
	short			showPilotCount;

	short			showPointCount;
	short			showChampCount;

    short           currSliderPos;
    short   		oldSliderPos;

	short			venomComplete;
    unsigned char	negTwist;
    unsigned char	oldNegTwist;

    signed char     negconPreset;
    signed char     oldNegPreset;
    signed char     normpadPreset;
    signed char     oldPadPreset;

	signed char		cdTrack;
    signed char     oldCdTrack;
	char			pad1;
	char			pad2;

	char 			trackNum;
	char 			actTrackNum;
	char			shipNum;
	char			ship2Num;

	char			class;
	char			team;
	char			team2;
	char			oppMenu;

	char			exitMenu;
	char			exit2Menu;
	char			gameType;
	char			NumTracks;

	char			Lives;
	char			Position;
	char			EndOfGame;
	char			gameComplete;

	char			continueConfirm;
	char			RapierClass;
	char			CheatMode;
    char    		cardPresent;
	
	char			prevLap;
	char			newLapRecord;
    char    		gotoMenu;
    char    		showGameOver;

    char            showBonusTrack;
    char            trackInc;
    char            lostGame;
	char			pad3;
} ConfigData;
#endif

#ifdef _DEFINE_SHIP_DATA_
typedef struct _ShipData
{
	/***** Graphics Attributes *****/
	short			highResTableIndex;						 	/* Index into shipShapes for high-res model */
	short			lowResTableIndex;							 	/* Index into shipShapes for low-res model */

	/***** Dynamic Attributes *****/
	TrackSection *nearTrkSect, *prevShipSect;
	int 			attr;												/* Ship attributes */
	int 			combatAttr;											/* Ship attributes */
	VECTOR 		unitVecNose;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		unitVecWing;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		ppivot;										 	/* 32bit ship position */
	VECTOR 		vpivot, apivot;							 	/* Velocity, accn vectors for ship */
	short 		hdg, pitch, roll;							 	/* Orientation attributes */
	short 		vhdg, vpitch, vroll;	
	short 		ahdg;						
	short 		mass;											 	/* Mass of ship */
 	VECTOR 		thrust;										  	/* Thrust vector */
	short 		r_brake_rot, l_brake_rot;					/* Air brakes */
	u_short 		speed;										  	/* speed */
	short 		thrust_mag;				 					/* Thrust magnitude */
	short 		max_thrust;								  	/* Miximum thrust */
	short 		collideCount;
	short 		headingInc, maxHeading;
	short 		resistance;
	short 		skid;

	/***** Remote Ship Attributes *****/
	short			fightBack, numtospeed	;
	u_short 		remoteThrustMag; 		 					/* Thrust magnitude */
	u_short 		remoteMaxThrust; 						  	/* Miximum thrust */
	short			DPASectDiff;	

	/***** Weapon Attributes *****/
	u_char 		weaponType;
	u_char 		targetShip;
	short  		haveFired;
	short			electroCount;
	short			revConCount;
	short			specialCount;

	/***** Race Control Attributes *****/
	int 			lapTime;
	char 			lapNo;
    char			prevLapNo;
	char 			position;
	char			pad1;
	short 		nearSecNo, prevSecNo ;
	int 			lapTimes[5];
	int 			updateCount;

	/***** Control Routines *****/
	void 			(*updateControl)(VECTOR* , struct ShipData*, Face*, int);
	void 			(*update)(struct ShipData *, Object**, int, struct WeaponData*);

} ShipData;
#endif

//-----------------------------------------------------------------------//

extern long				CommNetPlayers;
extern long				CommNetSocket;
extern long				CommMyShip;
extern char				gameType;
extern long				AllPlayersReady;
extern long				StartCount;
extern long				exit_status_on;
extern long				delta_frames_on;
extern int				ownShip;
extern int				serialShip;

//-----------------------------------------------------------------------//
extern char				GameID[ID_SIZE + 1];
extern struct _master_menu_packet	NetworkConfigTable;
extern struct _address				SortedMachineAddress[MAX_NET_MACHINES];
//-----------------------------------------------------------------------//

extern long				MAK_InitializeNetwork(void);
extern long				MAK_TerminateNetwork(void);
extern long				MAK_HandshakeNetwork(void);
extern long				MAK_CheckNetworkHandshake(void);
extern long				MAK_ResolveNetwork(void);
extern long				MAK_CheckNetworkResolve(void);
extern void				TestHardWirePacketSend(void);
extern int				MAK_address_compare(void *address1, void *address2);
extern long				MAK_SynchroniseNetwork(void);
extern long				MAK_CheckSyncPacket(void);
extern long				MAK_CheckAcknPacket(void);
extern void				MAK_MasterNetworkMenus(ConfigData *gameData);
extern void				MAK_SlaveNetworkMenus(ConfigData *gameData);
extern void				MAK_RaceNetworkMaster(struct _ShipData *shipInfo);
extern void				MAK_RaceNetworkSlave(struct _ShipData *shipInfo);
extern long				MAK_CheckPlayPacket(void);

//-----------------------------------------------------------------------//
