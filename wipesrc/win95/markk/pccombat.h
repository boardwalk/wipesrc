//-------------------------------------------------------------------

#define 	SLAVE					1
#define		MASTER					0
#define		SINGLE					2
#define		MASTERPOLL				3
#define		SLAVEDETECT				4
#define		WAITING					0
#define		PROCESSING				1
#define		INIT					2
#define		EXIT_RETRY				30
#define		TIMEOUT					50000
#define 	BR						2073600
#define 	DATA_PACKET_SIZE		168
//#define		SLAVE_TRANSMIT_SIZE		24
//#define		MASTER_RECEIVE_SIZE		24

#define		COMBATDEBUG				0
#define		PAUSE					0x30	//190
#define		CONT					0x31	//200
#define		RETIRE					0x32	//210
#define		QUIT				    0x33	//220
#define		NULLDATA				0x34	//230

#define		CRC_ERROR				0x39

#define		SIOCM_STP0  			0x00000000		/* Stop bits */
#define		SIOCM_STP1  			0x00000040
#define		SIOCM_STP15 			0x00000080
#define		SIOCM_STP2  			0x000000C0
#define		SIOCM_PARO  			0x00000000		/* Parity type */
#define		SIOCM_PARE  			0x00000020
#define		SIOCM_NOPAR 			0x00000000		/* Parity Enable */
#define		SIOCM_PAR   			0x00000010
#define		SIOCM_DAT5  			0x00000000		/* Data length */
#define		SIOCM_DAT6  			0x00000004
#define		SIOCM_DAT7  			0x00000008
#define		SIOCM_DAT8  			0x0000000C
#define		SIOCL_DTRON 			0x00000001		/* Control lines */
#define		SIOCL_RTSON 			0x00000002

//-------------------------------------------------------------------

#define 	WriteSIOData(b, l)		cwrite(CommSerialDevice, b, l)
#define 	ReadSIOData(b, l)		cread(CommSerialDevice, b, l)

//-------------------------------------------------------------------

extern void				_init_cont(void);
extern void				InitializeCombatLink(void);
extern void				MasterSlaveSIO(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData);
extern void				MenuMaster(ConfigData *gameData, int *currMenu);
extern void				CaptainBirdsEye(ConfigData *gameData, int *currMenu);
extern void				StartTwoPlayer(MusicPrims *musicPrims, SlideTextures *musicTex, SelectionData *selectData, char loadGames[][9], ConfigData *gameData);
extern void				SetSerialData(ShipData *shipIndex, Object **shipShapes);

//-------------------------------------------------------------------
