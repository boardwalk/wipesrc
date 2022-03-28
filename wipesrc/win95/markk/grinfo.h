//-------------------------------------------------------------------
// Header file to store line number info for each debugging var used
// (17/08/95)
//-------------------------------------------------------------------

/* Debugging display switches */
//#define				_GR_HUMAN_CAR_ON
//#define				_GR_LINK_CAR_ON
//#define				_GR_COMPUTER_CARS_ON
//#define				_GR_FRAME_COUNTER_ON
//#define				_GR_RECEIVE_PTRS_ON
//#define				_GR_READ_LOCK_ON
//#define				_GR_READ_COUNTER_ON
//#define				_GR_MACHINE_ID_ON
//#define				_GR_RPM_ON
//#define				_GR_SHIFT_ON
//#define				_GR_DRIVE_MODE_ON
//#define				_GR_GAME_STAT_ON
//#define				_GR_TIRE_MODE_ON
//#define				_GR_T_VALUES_ON
//#define				_GR_PAD_INFO_ON

//-------------------------------------------------------------------

#define					_FRAME_COUNTER		0*8
#define					_ID					0*8
#define					_RECEIVE_HEAD_PTR	1*8
#define					_RECEIVE_TAIL_PTR	2*8
#define					_MY_CAR				3*8
#define					_LINK_CAR			4*8
#define					_COMPUTER_CARS		10*8
#define					_READ_COUNTER		9*8
#define					_READ_LOCK			8*8
#define					_RPM				15*8
#define					_SHIFT				16*8
#define					_MODE				17*8
#define					_STAT				18*8
#define					_TIRE				19*8
#define					_C_LOCK				20*8
#define					_T_VALUE1			5*8
#define					_T_VALUE2			6*8
#define					_PAD_0				11*8
#define					_PAD_1				12*8

//-------------------------------------------------------------------

#define					dbg_point			WriteVGAString(0, 0, "DEBUG POINT");
#ifdef					FN_DEBUG
#define					fn_track(x)			fn_track1(x)
#define					fn_create			fn_file = fopen("FNTRACK", "wt");
#else
#define					fn_track(x)			;
#define					fn_create			;
#endif
#define					fn_track1(x)		WriteVGAString(0, 20*8, x); \
											fn_file = fopen("FNTRACK", "at"); \
											fprintf(fn_file, x); \
											fclose(fn_file);
											
//-------------------------------------------------------------------

extern char				DebugString[];
extern FILE				*fn_file;

//-------------------------------------------------------------------
