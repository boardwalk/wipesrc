/* Source File: vars.h */

/* Hardware Register Interface. */
union REGS				etc_regs;
struct SREGS			etc_sregs;

/* Interrupt Handling. */
unsigned short int	etc_OriginalInt09Selector;
unsigned long int		etc_OriginalInt09Offset;
void __far				(__far	*etc_JoystickService) ();
void 						(__interrupt __far	*old_timer) ();

/* Low-Level KeyCode Handling. */
signed char				etc_KeyboardHandler = 0;
signed char				etc_JoystickHandler = 0;
signed char				etc_MouseHandler = 0;
signed char				etc_TimerHandler = 0;
unsigned char			etc_ExtendedMode = 0;
unsigned char			etc_ExtendedFlag = 0;
unsigned char			etc_ScanCode = 0;
unsigned char			etc_Status = 0;
signed char				etc_KeyPressed = 0;
unsigned short int	etc_ExtKey = 0;
unsigned char			etc_Key = 0;
unsigned char			etc_ExitKey = 0;

/* Low-Level Mouse Handling. */
MOUSE						etc_Mouse1;
MOUSE						etc_Mouse2;

/* Low-Level Joystick Handling. */
unsigned short			etc_Joystick1CentreX;
unsigned short			etc_Joystick1CentreY;
unsigned short			etc_Joystick2CentreX;
unsigned short			etc_Joystick2CentreY;
JOYSTICK					etc_Joy1;
JOYSTICK					etc_Joy2;

/* Buffer pointers. */
char						*etc_TransferBuffer1;
char						*etc_TransferBuffer2;

/* Input Device Status Blocks. (5 bytes each) */
unsigned char			etc_KeyboardSwitch = _DEVICE_OFF;		/* id = 0 */
unsigned long int		etc_KeyboardRegister = 0;
unsigned char			etc_Joystick1Switch = _DEVICE_OFF;		/* id = 1 */
unsigned long int		etc_Joystick1Register = 0;
unsigned char			etc_Joystick2Switch = _DEVICE_OFF;		/* id = 2 */
unsigned long int		etc_Joystick2Register = 0;
unsigned char			etc_Mouse1Switch = _DEVICE_OFF;			/* id = 3 */
unsigned long int		etc_Mouse1Register = 0;
unsigned char			etc_Mouse2Switch = _DEVICE_OFF;			/* id = 4 */
unsigned long int		etc_Mouse2Register = 0;

/* Input Device Configuration. */
unsigned char			etc_InputDeviceSelect = 0;
unsigned char			etc_KeyboardConfig[_MAX_BUTTONS];
unsigned long int		etc_KeyboardMap[_MAX_KEYS * 2];
unsigned long int		etc_ExtendedKeyboardMap[_MAX_EXT_KEYS * 2];
JOYSTICK_CONFIG		etc_Joy1Config;
JOYSTICK_CONFIG		etc_Joy2Config;
MOUSE_CONFIG			etc_Mouse1Config;
MOUSE_CONFIG			etc_Mouse2Config;

/* Default Keyboard Configuration. */
unsigned short int	etc_DefaultKeyboardSet[_MAX_BUTTONS] = {
		0xFF49,
		0xFF51,
		0xFF49,
		0xFF51,
		_UNDEFINED,
		_UNDEFINED,
		0x0038,
		0x001D,
		0x001C, //enter
		_UNDEFINED,
		_UNDEFINED,
		0xFF45, //pause
		_UNDEFINED,
		0xFF4D, //right
		_UNDEFINED,
		0xFF4B }; //left

/* Default Joystick Configuration. */
unsigned long int		etc_DefaultJoystickSet[_MAX_JOY_SWITCHES] = {
		RDown,
		RLeft,
		LLeft,
		LRight,
		Stop,
/*		Play, */
		RUp,
		_UNDEFINED,
		_UNDEFINED };

/* Default Mouse Configuration. */
unsigned long int		etc_DefaultMouseSet[_MAX_MOUSE_SWITCHES] = {
		_UNDEFINED,
		_UNDEFINED,
		LLeft,
		LRight,
		RDown,
		_UNDEFINED,
		RLeft,
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
