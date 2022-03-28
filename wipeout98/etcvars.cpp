/* Source File: etcvars.c */
#include 						"\wipesrc\win95\markk\input.h"

/* Interrupt Handling. */
volatile unsigned short int		etc_OriginalInt09Selector;
volatile unsigned long int		etc_OriginalInt09Offset;

/* Low-Level KeyCode Handling. */
volatile signed char			etc_KeyboardHandler = 0;
volatile signed char			etc_JoystickHandler = 0;
volatile signed char			etc_MouseHandler = 0;
volatile signed char			etc_TimerHandler = 0;
volatile unsigned char			etc_ExtendedMode = 0;
volatile unsigned char			etc_ExtendedFlag = 0;
volatile unsigned char			etc_ScanCode = 0;
volatile unsigned char			etc_Status = 0;
volatile signed char			etc_KeyPressed = 0;
volatile unsigned short int		etc_ExtKey = 0;
volatile unsigned char			etc_Key = 0;
volatile unsigned char			etc_ExitKey = 0;
//volatile unsigned long int		etc_KeyboardCounter = 0;

/* Low-Level Mouse Handling. */
volatile MOUSE					etc_Mouse1;
volatile MOUSE					etc_Mouse2;

/* Low-Level Joystick Handling. */
volatile unsigned short			etc_Joystick1CentreX;
volatile unsigned short			etc_Joystick1CentreY;
volatile unsigned short			etc_Joystick2CentreX;
volatile unsigned short			etc_Joystick2CentreY;
volatile JOYSTICK				etc_Joy1;
volatile JOYSTICK				etc_Joy2;

/* Buffer pointers. */
volatile char					*etc_TransferBuffer1;
volatile char					*etc_TransferBuffer2;

/* Input Device Status Blocks. (5 bytes each) */
volatile unsigned char			etc_KeyboardSwitch = _DEVICE_OFF;		/* id = 0 */
volatile unsigned long int		etc_KeyboardRegister = 0;
volatile unsigned char			etc_Joystick1Switch = _DEVICE_OFF;		/* id = 1 */
volatile unsigned long int		etc_Joystick1Register = 0;
volatile unsigned char			etc_Joystick2Switch = _DEVICE_OFF;		/* id = 2 */
volatile unsigned long int		etc_Joystick2Register = 0;
volatile unsigned char			etc_Mouse1Switch = _DEVICE_OFF;			/* id = 3 */
volatile unsigned long int		etc_Mouse1Register = 0;
volatile unsigned char			etc_Mouse2Switch = _DEVICE_OFF;			/* id = 4 */
volatile unsigned long int		etc_Mouse2Register = 0;

/* Input Device Configuration. */
volatile unsigned char			etc_InputDeviceSelect = 0;
volatile unsigned char			etc_KeyboardConfig[_MAX_BUTTONS];
volatile unsigned long int		etc_KeyboardMap[_MAX_KEYS * 2];
volatile unsigned long int		etc_ExtendedKeyboardMap[_MAX_EXT_KEYS * 2];
volatile JOYSTICK_CONFIG		etc_Joy1Config;
volatile JOYSTICK_CONFIG		etc_Joy2Config;
volatile MOUSE_CONFIG			etc_Mouse1Config;
volatile MOUSE_CONFIG			etc_Mouse2Config;

/* Default Keyboard Configuration. */
#ifdef WIPEOUTPC
unsigned short etc_DefaultKeyboardSet[_MAX_BUTTONS] = {
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
unsigned short etc_DefaultKeyboardSet[_MAX_BUTTONS] = {
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
unsigned long int		etc_DefaultJoystickSet[_MAX_JOY_SWITCHES] = {
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
unsigned long int		etc_DefaultMouseSet[_MAX_MOUSE_SWITCHES] = {
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		_UNDEFINED,
		Cross,
		_UNDEFINED,
		Circle,
		_UNDEFINED };
		
/* Useful Strings. */
char	*buttonname[_MAX_BUTTONS] = {
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
