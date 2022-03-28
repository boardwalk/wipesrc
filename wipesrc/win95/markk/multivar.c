//-------------------------------------------------------------------
// ridge racer multi-player variables module
// this code now works with new Ridge Racer code (17/08/95)
//-------------------------------------------------------------------

#include			"..\origsrc\common.h"
#define				VAR_EXTERN
#include			"..\origsrc\var.h"
#include			"..\origsrc\sobj.h"

#define				_SIZE_T_DEFINED_
#include			"c:\watcom\h\stdio.h"
#include			"..\markk\multi.h"
#include			"..\markk\comm.h"

//-------------------------------------------------------------------

MULTI_CAR			mcar[M_CAR_MAX];
MULTI_CAR			*camera_mtrg;
RAW_PAD				transfer_buf[M_CAR_MAX];
VECTOR				black_mark_pos;

FILE				*DataFile;
FILE				*BinaryFile;
FILE				*fn_file;
long				CommDataFileOpen;

long				pause_machine;

long				CommMultiPlaySelect;
long				CommMultiPadFlag;
long				CommNumLinkedCars;
long				CommNetPlayers;
long				CommMyCar;
long				CommLinkCar;
long				CommMasterCar;
long				CommSlaveCar;
long				CommSerialConnectMethod;
long				CommSerialMachine;
long				CommSerialDevice;
long				CommSerialBaudRate;
long				CommSerialDataSize;
long				CommSerialParity;
long				CommSerialStopSize;
char				CommModemInitString[256];
char				CommModemDialupString[256];
long				CommFirstRead;
long				CommRandomSeed;
long				CommMultiPause;

//-------------------------------------------------------------------

