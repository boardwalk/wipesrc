/*
 *	test.c(今更テストモード)
 */

#include	"common.h"
#define		VAR_EXTERN
#include	"var.h"
#include	<sys\file.h>
#include	<asm.h>
#include	<kernel.h>


long	test_mode;
long	test_y;
long	test_sn;
long	test_rep;

void job_test0()
{
 	SetDispMask( FALSE );
	
	standard_init( 0x00, 0x00, 0x00 );
	job_cnt = 0;
	test_y = 0;
	test_mode = 0;
	test_sn = 1;
	test_rep = 0;
	
	job = JOB_TEST;
}

void test_select()
{
	long	a;
	char	**s;
	static	char	*m[] = {
		"EXIT",
		"ASCII",
		"KEY",
		"BACK UP CARD",
		"CD-DA",
		"SE",
		"BGM",
		"SAVE REPLAY",
		(char *)NULL,
	};
	
	if ( start_trig() ) {
		job = JOB_TITLE0;
	}
	
	/* disp */
	a = 0;
	for (s=&m[0];*s!=NULL;s++,a++) {
		add_str( 12*8, (a+5)*8, *s, WHITE );
	}
	
	add_str( 10*8, (5+test_y)*8, ">", (job_cnt & 0x08) ? WHITE : RED );
	
	add_str( 4*8, 25*8, "PUSH X TO DECIDE", RED );
	
	
	if ( pad_info.trig & PAD_A ) {
		if (test_y > 0) test_y--;
	}
	if ( pad_info.trig & PAD_B ) {
		if (test_y < a-1) test_y++;
	}
	if ( pad_info.trig & PAD_F ) {
		test_mode = test_y;
		if (test_mode == 0) job = JOB_TITLE0;
	}
	
	
}

void test_ascii()
{
	long	i,j;
	char	buf[4];
	
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_mode = 0;
	
	add_str( 9*8,  4*8, "ASCII CHARACTERS CHECK", WHITE );
	
	for (j=0;j<4;j++) {
		for (i=0;i<16;i++) {
			buf[0] = ' '+i+j*16;	buf[1] = 0;
			add_str( (4+i)*8, (j+ 8)*8, buf, WHITE );
			add_str( (4+i)*8, (j+13)*8, buf, RED   );
		}
	}
}

void test_key()
{
	RAW_PAD		*p = &pad_buf[0];
	long	i,d;
	char	buf[256];
	
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_mode = 0;
	
	add_str( 15*8,  4*8, "KEY TEST", WHITE );
	
	add_str( 4*8,  9*8, "           FEDCBA9876543210", WHITE );
	add_str( 4*8, 10*8, "PAD DATA : ", WHITE );
	for (i=0,d=0x8000;i<16;i++,d>>=1) {
		add_str( (4+11+i)*8, 10*8, (pad_info.data & d) ? "*" : ".", WHITE );
	}
	
	sprintf( buf, "CH0 : %02x   HANDLE : %02x", p->ch[0],pad_info.handle );
	add_str( 4*8, 14*8, buf, WHITE );
	
	sprintf( buf, "CH1 : %02x   GAS    : %02x", p->ch[1],pad_info.gas );
	add_str( 4*8, 15*8, buf, WHITE );
	
	sprintf( buf, "CH2 : %02x   BRAKE  : %02x", p->ch[2],pad_info.brake );
	add_str( 4*8, 16*8, buf, WHITE );
	
	sprintf( buf, "CH3 : %02x   OPTION : %02x", p->ch[3],pad_info.dummy );
	add_str( 4*8, 17*8, buf, WHITE );
}

void test_bu()
{
	long	i,j;
	char	buf[40],buf2[40],buf3[40];
	long	ret,ret2;
	static	char	msg[40] = { "STATUS MESSAGE WINDOW" };
	struct	DIRENTRY	bu_dir;
	
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_mode = 0;
	
	add_str( 11*8,  4*8, "BACK UP CARD TEST", WHITE );
	
	for (i=0;i<6;i++) {
		sprintf( buf, "%1d %s %s",i,make_time_str( buf2, today[i].time ),
														&today[i].name[0]);
		add_str( 4*8, (6+i)*8, buf, WHITE );
	}
	
	for (i=0;i<4;i++) {
		sprintf( buf, "%1d %s %s",i,
				make_time_str( buf2, best_total[i] ),
				make_time_str( buf3, best_lap  [i] ) );
		add_str( 4*8, (15+i)*8, buf, WHITE );
	}
	
	
	
	if ( pad_info.trig & PAD_A ) {
		ret = format( "bu00:" );
		sprintf( msg, "FORMAT [BU00] ... %d", ret );
	}
	
	if ( pad_info.trig & PAD_B ) {
		ret = open( "bu00:TEST", O_CREAT|(1<<16) );
		ret2 = close( ret );
		sprintf( msg, "OPEN [BU00] %d CLOSE %d", ret, ret2 );
	}
	
	if ( pad_info.trig & PAD_C ) {
		ret = firstfile( "bu00:*", &bu_dir );
		if (ret != 0) {
			i = 0;
			do {
				printf("%2d : %s\n",i++,bu_dir.name);
			} while ( nextfile( &bu_dir ) );
		} else {
			printf("file no exist.\n");
		}
	}
	
	
	add_str( 2*8, 20*8, msg, WHITE );
}

void test_cd()
{
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_mode = 0;
	
	add_str( 15*8,  4*8, "CD-DA TEST", WHITE );
	
}

void test_se()
{
	char	buf[120];
	char	num[4];
	static	short	h;
	
	if ( pad_info.trig & PAD_C ) {
		if (test_sn > 1) test_sn--;
	}
	if ( pad_info.trig & PAD_D ) {
		if (test_sn < SE_MAX) test_sn++;
	}
	if ( pad_info.trig & PAD_F ) {
		h = se_play( test_sn );
	}
	if ( pad_info.trig & PAD_G ) {
		se_all_stop();
	}
	
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_mode = 0;
	
	add_str( 11*8,  4*8, "SOUND EFFECTS TEST", WHITE );
	
	num[0] = '0' + test_sn / 100;
	num[1] = '0' + test_sn / 10 % 10;
	num[2] = '0' + test_sn % 10;
	num[3] = 0;
	add_str( 12*8,  6*8, "SE #", WHITE );
	add_str( 16*8,  6*8, num, WHITE );
}

void test_bgm()
{
}

void test_save_rep()
{
	static	long	ret;
	
	if (test_rep == 0) {
		ret = write_replay();
		test_rep = 1;
	}
	
	add_str( 4*8, 25*8, "PUSH START TO EXIT", RED );
	if ( pad_info.trig & PAD_H ) test_rep = test_mode = 0;
	
	add_str( 11*8,  4*8, "SAVE REPLAY DATA", WHITE );
	
	add_str( 12*8,  6*8, ret ? "SAVE OK!" : "SAVE ERROR!", ret ? WHITE : RED );
}

void     (*test_table[])() = {
	test_select,	/*  0 */
	test_ascii,		/*  1 */
	test_key,		/*  2 */
	test_bu,		/*  3 */
	test_cd,		/*  4 */
	test_se,		/*  5 */
	test_bgm,		/*  6 */
	test_save_rep,	/*  7 */
};


void job_test()
{
	long	y;
	
	if (job_cnt++ == 2) SetDispMask( TRUE );
	
	add_str( 15*8,  3*8, "TEST MODE", RED );
	
	test_table[ test_mode ]();
	
meter( 0xff, 0x00, 0x00 );
}
