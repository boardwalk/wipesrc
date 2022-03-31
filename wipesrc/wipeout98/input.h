volatile /* Source File: libetc.h */
#include	"scancode.h"

/* General Equates. */
#define		_MAX_KEYS				128
#define		_MAX_EXT_KEYS			128
#define		_MAX_BUTTONS			32
#define		_MAX_JOY_SWITCHES		8
#define		_MAX_MOUSE_SWITCHES		8
#define		_UNDEFINED				0
#define		_JOY_ERROR				10

#define		SONY_PAD				0x41
#define		NEGCON					0x23

#define		OK						0
#define		NG						0xFF

/* Bit Values for the joypad Register. */
#define		BotRight				0
#define		BotLeft					1
#define		TopRight				2
#define		TopLeft					3
#define		RUp						4
#define		RRight					5
#define		RDown					6
#define		RLeft					7
#define		Stop					8
#define		Play					11
#define		LUp						12
#define		LRight					13
#define		LDown					14
#define		LLeft					15

/* Bit Values for the joypad Register. */
#define		FrontRightBottom		BotRight
#define		FrontLeftBottom			BotLeft
#define		FrontRightTop			TopRight
#define		FrontLeftTop			TopLeft
#define		Triangle				RUp
#define		Circle					RRight
#define		Cross					RDown
#define		Square					RLeft
#define		Select					Stop
#define		Start					Play
#define		Up						LUp
#define		Right					LRight
#define		Down					LDown
#define		Left			 		LLeft

/* I/O Ports. */
#define		_INTERRUPT1_PORT		0x20
#define		_KEYBOARD_PORT			0x60
#define		_STATUS_PORT			0x64
#define		_INTERRUPT2_PORT		0xA0
#define		_JOYSTICK_PORT			0x200

/* Interrupts. */
#define		_TIMER_INT				0x08
#define		_KEYBOARD_INT			0x09
#define		_BIOS_INT				0x10
#define		_JOYSTICK_INT			0x15
#define		_DOS_INT				0x21
#define		_DPMI_INT				0x31
#define		_MOUSE_INT				0x33

#define		_MOUSE_STACK			1024

/* PC Input Device Selection Bits. */
#define		_KEYBOARD				1
#define		_JOYSTICK1				2
#define		_JOYSTICK2				3
#define		_MOUSE1					4
#define		_MOUSE2					5

#define		_DEVICE_OFF				0
#define		_DEVICE_ON				1
#define		_DEVICE_SIZE			5

#define		_XT						0
#define		_AT						1
#define		_MF_II					2
#define		_PAUSE_ID				3

/* Function Prototypes. */
extern void							InstallProtectedModeKeyboardHandler(void);
extern void							SaveProtectedModeKeyboardHandler(void);
extern void							RestoreProtectedModeKeyboardHandler(void);
extern void							InstallProtectedModeMouseHandler(void);
extern void							RestoreProtectedModeMouseHandler(void);
extern void							InstallProtectedModeJoystickHandler(void);
extern void							RestoreProtectedModeJoystickHandler(void);
extern void							ConfigureKeyboard(void);
extern void							ConfigureMouse1(void);
extern void							ConfigureMouse2(void);
extern void							ConfigureJoystick1(void);
extern void							ConfigureJoystick2(void);
extern void							AssignKey(uint32_t button);
extern uint16_t			GetKey(void);
extern void							SetCursorPosition(uint8_t row, uint8_t col);
extern void							init_timer(void);
extern void							restore_timer(void);

extern void       			*GetStack(void);
extern void             			SetStack(char  *);
#ifdef DOS_VERSION
extern void __interrupt	__far		pmkey(void);
#else
extern void pmkey(char, char);
#endif
extern void							ManageScanCode(char);
//extern static void __loadds __far 	pmmouse(int16_t mask, int16_t state, int16_t col, int16_t row,
//										signed int16_t mickey_h, signed int16_t mickey_v);
extern void		pmjoy(void);
//extern void __loadds __interrupt __far	pmtimer(void);


/* LIBETC Functions. */
extern int32_t							PadInit(uint32_t mode);
extern int32_t							PadRead(uint16_t id);
extern int32_t							PadStatus(uint16_t mode);

/* LIBAPI Functions. */
extern int32_t							InitPAD(void *bufA, int32_t lenA, void *bufB, int32_t lenB);
extern int32_t							StartPAD(void);
extern void							StopPAD(void);

/* Type/Structure Definitions. */
typedef struct {
	uint16_t 	jX;
	uint16_t 	jY;
	uint16_t 	jDir;
	uint16_t 	jButton1;
	uint16_t 	jButton2;
} JOYSTICK;

typedef struct {
	uint16_t 	mX;
	uint16_t 	mY;
	uint16_t 	mDir;
	uint16_t 	mButton1;
	uint16_t 	mButton2;
	uint16_t 	mButton3;
} MOUSE;

typedef struct {
	int32_t	JoyUp;
	int32_t	JoyDown;
	int32_t	JoyLeft;
	int32_t	JoyRight;
	int32_t	JoyButton1;
	int32_t	JoyButton2;
	int32_t	JoyButton3;
	int32_t	JoyButton4;
} JOYSTICK_CONFIG;

typedef struct {
	int32_t	MouseUp;
	int32_t	MouseDown;
	int32_t	MouseLeft;
	int32_t	MouseRight;
	int32_t	MouseLeftButton;
	int32_t	MouseMiddleButton;
	int32_t	MouseRightButton;
	int32_t	MouseDummy;
} MOUSE_CONFIG;

/* Variable Declarations. */
/* Hardware Register Interface. */
extern volatile union REGS				etc_regs;
extern volatile struct SREGS			etc_sregs;

/* Low-Level KeyCode Handling. */
extern volatile int8_t				etc_KeyboardHandler;
extern volatile int8_t				etc_JoystickHandler;
extern volatile int8_t				etc_MouseHandler;
extern volatile int8_t				etc_TimerHandler;
extern volatile uint8_t			etc_ExtendedMode;
extern volatile uint8_t			etc_ExtendedFlag;
extern volatile uint8_t			etc_ScanCode;
extern volatile uint8_t			etc_Status;
extern volatile int8_t				etc_KeyPressed;
extern volatile uint16_t 		etc_ExtKey;
extern volatile uint8_t			etc_Key;
extern volatile uint8_t			etc_ExitKey;
extern volatile uint32_t 		etc_KeyboardCounter;

/* Low-Level Mouse Handling. */
extern volatile MOUSE					etc_Mouse1;
extern volatile MOUSE					etc_Mouse2;

/* Low-Level Joystick Handling. */
extern volatile uint16_t			etc_Joystick1CentreX;
extern volatile uint16_t			etc_Joystick1CentreY;
extern volatile uint16_t			etc_Joystick2CentreX;
extern volatile uint16_t			etc_Joystick2CentreY;
extern volatile JOYSTICK				etc_Joy1;
extern volatile JOYSTICK				etc_Joy2;

/* Buffer pointers. */
extern volatile char					*etc_TransferBuffer1;
extern volatile char					*etc_TransferBuffer2;

/* Input Device Status Blocks. (5 bytes each) */
extern volatile uint8_t			etc_KeyboardSwitch;		/* id = 0 */
extern volatile uint32_t 		etc_KeyboardRegister;
extern volatile uint8_t			etc_Joystick1Switch;		/* id = 1 */
extern volatile uint32_t 		etc_Joystick1Register;
extern volatile uint8_t			etc_Joystick2Switch;		/* id = 2 */
extern volatile uint32_t  		etc_Joystick2Register;
extern volatile uint8_t			etc_Mouse1Switch;			/* id = 3 */
extern volatile uint32_t 		etc_Mouse1Register;
extern volatile uint8_t			etc_Mouse2Switch;			/* id = 4 */
extern volatile uint32_t 		etc_Mouse2Register;

/* Input Device Configurations. */
extern volatile uint8_t			etc_InputDeviceSelect;
extern volatile uint8_t			etc_KeyboardConfig[_MAX_BUTTONS];
extern volatile uint32_t 		etc_KeyboardMap[_MAX_KEYS * 2];
extern volatile uint32_t 		etc_ExtendedKeyboardMap[_MAX_EXT_KEYS * 2];
extern volatile JOYSTICK_CONFIG			etc_Joy1Config;
extern volatile JOYSTICK_CONFIG			etc_Joy2Config;
extern volatile MOUSE_CONFIG			etc_Mouse1Config;
extern volatile MOUSE_CONFIG			etc_Mouse2Config;

/* Default Input Device Configurations. */
extern uint16_t 				etc_DefaultKeyboardSet[_MAX_BUTTONS];
extern uint32_t 				etc_DefaultJoystickSet[_MAX_JOY_SWITCHES];
extern uint32_t 				etc_DefaultMouseSet[_MAX_MOUSE_SWITCHES];

/* Strings. */
extern const char								*buttonname[_MAX_BUTTONS];
