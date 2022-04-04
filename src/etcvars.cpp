#include 						"input.h"

/* Interrupt Handling. */
volatile uint16_t		etc_OriginalInt09Selector;
volatile uint32_t		etc_OriginalInt09Offset;

/* Low-Level KeyCode Handling. */
volatile int8_t			etc_KeyboardHandler = 0;
volatile int8_t			etc_JoystickHandler = 0;
volatile int8_t			etc_MouseHandler = 0;
volatile int8_t			etc_TimerHandler = 0;
volatile uint8_t			etc_ExtendedMode = 0;
volatile uint8_t			etc_ExtendedFlag = 0;
volatile uint8_t			etc_ScanCode = 0;
volatile uint8_t			etc_Status = 0;
volatile int8_t			etc_KeyPressed = 0;
volatile uint16_t		etc_ExtKey = 0;
volatile uint8_t			etc_Key = 0;
volatile uint8_t			etc_ExitKey = 0;
//volatile uint32_t		etc_KeyboardCounter = 0;

/* Low-Level Mouse Handling. */
volatile MOUSE					etc_Mouse1;
volatile MOUSE					etc_Mouse2;

/* Low-Level Joystick Handling. */
volatile uint16_t			etc_Joystick1CentreX;
volatile uint16_t			etc_Joystick1CentreY;
volatile uint16_t			etc_Joystick2CentreX;
volatile uint16_t			etc_Joystick2CentreY;
volatile JOYSTICK				etc_Joy1;
volatile JOYSTICK				etc_Joy2;

/* Buffer pointers. */
volatile char					*etc_TransferBuffer1;
volatile char					*etc_TransferBuffer2;

/* Input Device Status Blocks. (5 bytes each) */
volatile uint8_t			etc_KeyboardSwitch = _DEVICE_OFF;		/* id = 0 */
volatile uint32_t		etc_KeyboardRegister = 0;
volatile uint8_t			etc_Joystick1Switch = _DEVICE_OFF;		/* id = 1 */
volatile uint32_t		etc_Joystick1Register = 0;
volatile uint8_t			etc_Joystick2Switch = _DEVICE_OFF;		/* id = 2 */
volatile uint32_t		etc_Joystick2Register = 0;
volatile uint8_t			etc_Mouse1Switch = _DEVICE_OFF;			/* id = 3 */
volatile uint32_t		etc_Mouse1Register = 0;
volatile uint8_t			etc_Mouse2Switch = _DEVICE_OFF;			/* id = 4 */
volatile uint32_t		etc_Mouse2Register = 0;

/* Input Device Configuration. */
volatile uint8_t			etc_InputDeviceSelect = 0;
volatile uint8_t			etc_KeyboardConfig[_MAX_BUTTONS];
volatile uint32_t		etc_KeyboardMap[_MAX_KEYS * 2];
volatile uint32_t		etc_ExtendedKeyboardMap[_MAX_EXT_KEYS * 2];
volatile JOYSTICK_CONFIG		etc_Joy1Config;
volatile JOYSTICK_CONFIG		etc_Joy2Config;
volatile MOUSE_CONFIG			etc_Mouse1Config;
volatile MOUSE_CONFIG			etc_Mouse2Config;

/* Default Keyboard Configuration. */
#ifdef WIPEOUTPC
uint16_t etc_DefaultKeyboardSet[_MAX_BUTTONS] = {
		_Z_KEY,	   			/* FrontLeftBottom	*/	// left airbreak
		_X_KEY,	  			/* FrontRightBottom	*/	// right airbreak
		_F1_KEY,		   	/* FrontRightTop	*/	// Draw distance
		_ESC_KEY,		   	/* FrontLeftTop		*/
		_TAB_KEY,		  	/* Triangle			*/ 	// change view
		_LEFT_ALT_KEY,	   	/* Circle			*/	// shoot weapon
		_LEFT_CTRL_KEY,   	/* Cross			*/ 	// accelerate
		_F2_KEY,			/* Square			*/	// Screen size
		_ENTER_KEY,		   	/* Select			*/	// editor
		_F3_KEY,			/* Unused			*/	// Resolution
		_F4_KEY,			/* Unused			*/	// Sky
		_P_KEY,		   		/* Start			*/ 	// pause
		_UP_KEY,		   	/* Up				*/	// up
		_RIGHT_KEY,		   	/* Right			*/ 	// right
		_DOWN_KEY,		   	/* Down				*/ 	// down
		_LEFT_KEY };	   	/* Left			 	*/ 	// left
#else
uint16_t etc_DefaultKeyboardSet[_MAX_BUTTONS] = {
		_RIGHT_KEY,	   		/* FrontRightBottom	*/
		_LEFT_KEY,		   	/* FrontLeftBottom	*/
		_PAGEUP_KEY,	   	/* FrontRightTop	*/
		_HOME_KEY,		   	/* FrontLeftTop		*/
		_Q_KEY,			  	/* Triangle			*/
		_S_KEY,			   	/* Circle			*/
		_LEFT_ALT_KEY,	   	/* Cross			*/
		_LEFT_CTRL_KEY,	   	/* Square			*/
		_ENTER_KEY,		   	/* Select			*/
		_ENTER_KEY,			/* Menu select key	*/
		_ESC_KEY,			/* Menu cancel key	*/
		_P_KEY,		   		/* Start			*/
		_UP_KEY,		   	/* Up				*/
		_RIGHT_KEY,		   	/* Right			*/
		_DOWN_KEY,		   	/* Down				*/
		_LEFT_KEY };	   	/* Left			 	*/
#endif
/* Default Joystick Configuration. */
uint32_t		etc_DefaultJoystickSet[_MAX_JOY_SWITCHES] = {
//		LUp,
//		LDown,
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		Circle,
		Cross,
		_UNDEFINED,
		_UNDEFINED };

/* Default Mouse Configuration. */
uint32_t		etc_DefaultMouseSet[_MAX_MOUSE_SWITCHES] = {
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		Cross,
		_UNDEFINED,
		Circle,
		_UNDEFINED };

/* Useful Strings. */
const char	*buttonname[_MAX_BUTTONS] = {
		"FrontRightBottom",
		"FrontLeftBottom",
		"FrontRightTop",
		"FrontLeftTop",
		"Triangle",
		"Circle",
		"Cross",
		"Square",
		"Select",
		"NotUsed",
		"NotUsed",
		"Start",
		"Up",
		"Right",
		"Down",
		"Left" };
