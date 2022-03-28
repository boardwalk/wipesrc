/* VGA Text Display routines, useful for debugging. */
#define		VAR_EXTERN
#include	"..\origsrc\common.h"
#include	"..\origsrc\var.h"

#define		_SIZE_T_DEFINED_
#include	"c:\watcom\h\stdio.h"

#include	"..\markk\comio.h"
#include	"..\markk\comm.h"
#define		_STATIC_OBJ_UNDEFINED_
#include	"..\markk\multi.h"
#include	"..\markk\grtext.h"
#include	"..\markk\grinfo.h"

long		framecounter;
char		MarkString[256];

/* Draw a null-terminated string using my VGA bitmap font. */
void	WriteVGAString(char x, char y, char *Buffer)
{
	char	i = 0;
	while (Buffer[i] != '\0') {
		WriteVGACell(i + x, y, Buffer[i]);
		i++;
	}
}

void	WriteVGACell(char x, char y, char c)
{
	int		i,j;
	char	*VGACell = (char *) 0x000A0000;
	char	mask;
	CELL	*rcell;
	
	rcell = &DebugFontTable[c];
	VGACell += (y * _SCREEN_WIDTH) + (x * _CELL_WIDTH);

	for (i=0; i<_CELL_HEIGHT; i++) {
		mask = 0x80;
 		for (j=0; j<_CELL_WIDTH; j++) {
			if((rcell->RowByte[i] & mask) != 0)	*VGACell = _COLOUR_PIXEL_BYTE;
			else *VGACell = _ZERO_PIXEL_BYTE;
			mask >>= 1;
			VGACell++;
		}
		VGACell += _SCREEN_WIDTH - _CELL_WIDTH;
	}	
}

void	HeadTest2(void)
{
		int	i;
// MARK TESTING SERIAL / NETWORK MODIFICATIONS: 14/06/95
//----------------------------------------------------------------------------
// Debug Testing
#ifdef	_GR_FRAME_COUNTER_ON
		WriteVGAString(0,_FRAME_COUNTER, "frame:");
		framecounter = (long) f_cnt;
		ltoa(framecounter, MarkString, 16);
		WriteVGAString(10, _FRAME_COUNTER, MarkString);
#endif
#ifdef	_GR_HUMAN_CAR_ON
		/* My Car. */
		WriteVGAString(0,_MY_CAR, "mycar");
		WriteVGAString(6,_MY_CAR, "x:");
		ltoa(mycar[0].pos.vx, MarkString, 16);
		WriteVGAString(8, _MY_CAR, MarkString);
		WriteVGAString(16,_MY_CAR, "y:");
		ltoa(mycar[0].pos.vy, MarkString, 16);
		WriteVGAString(18, _MY_CAR, MarkString);
		WriteVGAString(26,_MY_CAR, "z:");
		ltoa(mycar[0].pos.vz, MarkString, 16);
		WriteVGAString(28, _MY_CAR, MarkString);
#endif
#ifdef	_GR_MODEL_ON
		ltoa((long) mycar[0].model, MarkString, 16);
		WriteVGAString(0, 20*8, "model:");
		WriteVGAString(10, 20*8, MarkString);
#endif
#ifdef	_GR_LINK_CAR_ON
		/* My Car. */
		WriteVGAString(0,_LINK_CAR, "ccar0");
		WriteVGAString(6,_LINK_CAR, "x:");
		ltoa(c_car[0].pos.vx, MarkString, 16);
		WriteVGAString(8, _LINK_CAR, MarkString);
		WriteVGAString(16,_LINK_CAR, "y:");
		ltoa(c_car[0].pos.vy, MarkString, 16);
		WriteVGAString(18, _LINK_CAR, MarkString);
		WriteVGAString(26,_LINK_CAR, "z:");
		ltoa(c_car[0].pos.vz, MarkString, 16);
		WriteVGAString(28, _LINK_CAR, MarkString);
#endif
#ifdef	_GR_COMPUTER_CARS_ON
		for (i=0; i<C_CAR_MAX; i++) {
			WriteVGAString(0,_COMPUTER_CARS+(i*8), "car");
			ltoa(i, MarkString, 10);
			WriteVGAString(3,_COMPUTER_CARS+(i*8), MarkString);
			WriteVGAString(6,_COMPUTER_CARS+(i*8), "x:");
			ltoa(c_car[i].pos.vx, MarkString, 16);
			WriteVGAString(8,_COMPUTER_CARS+(i*8), MarkString);
			WriteVGAString(16,_COMPUTER_CARS+(i*8), "y:");
			ltoa(c_car[i].pos.vy, MarkString, 16);
			WriteVGAString(18,_COMPUTER_CARS+(i*8), MarkString);
			WriteVGAString(26,_COMPUTER_CARS+(i*8), "z:");
			ltoa(c_car[i].pos.vz, MarkString, 16);
			WriteVGAString(28,_COMPUTER_CARS+(i*8), MarkString);
		}
#endif
#ifdef	_GR_MACHINE_ID_ON
		if(CommSerialMachine == _LEAD) WriteVGAString(0, _ID, "lead");
		if(CommSerialMachine == _FOLLOW) WriteVGAString(0, _ID, "follow");
#endif
#ifdef	_GR_RPM_ON
		WriteVGAString(0, _RPM, "rpm:");
		ltoa(mycar[0].u.m.rpm, MarkString, 16);
		WriteVGAString(5, _RPM, MarkString);
#endif	
#ifdef	_GR_SHIFT_ON
		WriteVGAString(0, _SHIFT, "gear:");
		ltoa(mycar[0].u.m.shift, MarkString, 16);
		WriteVGAString(7, _SHIFT, MarkString);
#endif
#ifdef	_GR_DRIVE_MODE_ON
		WriteVGAString(0, _MODE, "mode:");
		ltoa(mycar[0].u.m.drive_mode, MarkString, 16);
		WriteVGAString(7, _MODE, MarkString);
#endif
#ifdef	_GR_GAME_STAT_ON
		WriteVGAString(0, _STAT, "stat:");
		ltoa(mcar[0].game_stat, MarkString, 16);
		WriteVGAString(7, _STAT, MarkString);
#endif
#ifdef	_GR_TIRE_MODE_ON
		WriteVGAString(0, _TIRE, "tire:");
		ltoa(mycar[0].u.m.tire_mode, MarkString, 16);
		WriteVGAString(7, _TIRE, MarkString);
#endif
#ifdef	_GR_PAD_INFO_ON
		WriteVGAString(0, _PAD_0, "pad0:");
		ltoa(pad_info[0].trig, MarkString, 16);
		WriteVGAString(7, _PAD_0, MarkString);
		WriteVGAString(0, _PAD_1, "pad1:");
		ltoa(pad_info[1].trig, MarkString, 16);
		WriteVGAString(7, _PAD_1, MarkString);
#endif
#ifdef	_GR_RECEIVE_PTRS_ON
		WriteVGAString(0, _RECEIVE_HEAD_PTR, "rec head:");
		ultoa(com_ReceiveHead, MarkString, 16);
		WriteVGAString(10, _RECEIVE_HEAD_PTR, MarkString);
		WriteVGAString(0, _RECEIVE_TAIL_PTR, "rec tail:");
		ultoa(com_ReceiveTail, MarkString, 16);
		WriteVGAString(10, _RECEIVE_TAIL_PTR, MarkString);
#endif
}

void	MAKdelay(void)
{
	size_t	i,j,k;
	
	for(i=0; i<10000; i++)
		for(j=0; j<10; j++) k=0;
}


void	OutputCarDataFile(void)
{
	if(mcar[CommMyCar].game_stat >= 4) CloseCarDataFile();
	else if(mcar[CommLinkCar].game_stat >= 4) CloseCarDataFile();
	else {
		fprintf(DataFile, "\n");
		fprintf(DataFile, "Job Cnt: %ld\n", job_cnt);
		fprintf(DataFile, "Pad Info (0): %ld\n", pad_info[0].trig);
		fprintf(DataFile, "Pad Info (1): %ld\n", pad_info[1].trig);

		fprintf(DataFile, "MyCar Pos: x:%ld\tz:%ld\n", mycar[0].pos.vx, mycar[0].pos.vz);
		fprintf(DataFile, "MyCar Pos: t:%ld\ts:%ld\n", mycar[0].t, mycar[0].s);
		fprintf(DataFile, "MyCar Game Stat: %ld\n", mcar[0].game_stat);

		fprintf(DataFile, "HdCar Pos: x:%ld\tz:%ld\n", c_car[0].pos.vx, c_car[0].pos.vz);
		fprintf(DataFile, "HdCar Pos: t:%ld\ts:%ld\n", c_car[0].t, c_car[0].s);
		fprintf(DataFile, "HdCar Game Stat: %ld\n", mcar[1].game_stat);
	}
}

//---------------------------------------------------------------------

void	OpenCarDataFile(void)
{
	DataFile = fopen("car.dat", "wt");
}

//---------------------------------------------------------------------

void	CloseCarDataFile(void)
{
	fclose(DataFile);	
}

//---------------------------------------------------------------------

void	OpenBinaryCarFile(void)
{
	if(CommSerialMachine == _LEAD) {
		BinaryFile = fopen("lead.bin", "ab");
	}
	if(CommSerialMachine == _FOLLOW) {
		BinaryFile = fopen("follow.bin", "ab");
	}
	CommDataFileOpen = 0;
}

//---------------------------------------------------------------------

void	CloseBinaryCarFile(void)
{
	fclose(BinaryFile);
}

//---------------------------------------------------------------------

void	OutputBinaryCarData(void)
{
	OpenBinaryCarFile();
	if(CommDataFileOpen == 0) {
		if((mcar[CommMyCar].game_stat >= 4) || (mcar[CommLinkCar].game_stat >= 4)) CloseBinaryCarFile();
		else {
				// mster car data
				fwrite(&mcar[0], sizeof(MULTI_CAR), 1, BinaryFile);
				fwrite(mcar[0].c, sizeof(CAR), 1, BinaryFile);
				fwrite(&pad_info[mcar[0].pad], sizeof(PAD_INFO), 1, BinaryFile);

				// slave car data
				fwrite(&mcar[1], sizeof(MULTI_CAR), 1, BinaryFile);
				fwrite(mcar[1].c, sizeof(CAR), 1, BinaryFile);
				fwrite(&pad_info[mcar[1].pad], sizeof(PAD_INFO), 1, BinaryFile);
		}
	}
	CloseBinaryCarFile();
}