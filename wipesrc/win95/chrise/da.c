
#define		MINUS_SEEK		(15-5)

/*
BGM 2 3 4 5 6 7
OPENING 8
SELECT  9
TIMEUP  10
WINNING 11
*/

static	short	bgm_vol[] = {
	0x00,	0x00,
	0x50,	0x58,	0x58,	0x60,
	0x58,	0x60,	0x58,	0x58,
	0x58,	0x58,	0x60,	0x7f,
};

long	last_seek;

#include <sys/types.h>
#include <libetc.h>
#include <libsnd.h>
#include <r3000.h>
#include <asm.h>
#include <kernel.h>
#include <libcd.h>

#ifndef	ON
#define	ON	1
#endif
#ifndef	OFF
#define	OFF	0
#endif

#define	DA_MAX	16		/* CD の曲数 */

#define	PLAY	0
#define	STOP	1
#define	PAUSE	2
#define	DEPAUSE	3
#define	SEEK	4

static	void cd_play_main(void);
static	void cd_stop_main(void);
static	void cd_pause_main(void);
static	void cd_depause_main(void);

struct CdlPos	loc[DA_MAX];	/* loc[0] にはトータル時間が入る */

short	current_DA = -1;
short	fade_effect = 0;
short	ntoc;
CdlATV	atv, max;

char	CdJob = -1;
char	CdInit_f;
char	CdPlay_f;
char	CdStop_f;
char	CdPause_f;
char	CdDePause_f;

long	bbb;

int		drive;		//TEMP variable to let the F***er compile AJY

void cd_init(void)
{
    u_char	param[4];

    CdInit();	

    CdInit_f = ON;

	last_seek = 0;

    max.val0 = 0x7f;
	max.val1 = 0x00;
	max.val2 = 0x7f;
	max.val3 = 0x00;
}

void cd_max(unsigned char val0, unsigned char val1, unsigned char val2, unsigned char val3)
 {
  max.val0 = val0;
  max.val1 = val1;
  max.val2 = val2;
  max.val3 = val3;
 }


void cd_mix(unsigned char val0, unsigned char val1, unsigned char val2, unsigned char val3)
 {
  atv.val0 = val0;
  atv.val1 = val1;
  atv.val2 = val2;
  atv.val3 = val3;
  CdMix(&atv);
 }

void to10()
 {
  long	i,n;
	
  for (i=2;i<=ntoc;i++)
   {
	n = loc[i].sector;
	loc[i].sector = (n >> 4)*10 + (n & 0xf);
	n = loc[i].second;
	loc[i].second = (n >> 4)*10 + (n & 0xf);
	n = loc[i].minute;
	loc[i].minute = (n >> 4)*10 + (n & 0xf);
   }
 }

void to16()
 {
  long	i,n;
	
  for (i=2;i<=ntoc;i++)
   {
    n = loc[i].sector;
    loc[i].sector = (n/10)*16 + (n%10);
    n = loc[i].second;
    loc[i].second = (n/10)*16 + (n%10);
    n = loc[i].minute;
    loc[i].minute = (n/10)*16 + (n%10);
   }
 }

void cd_loop(void)
 {
  u_char	result[8];
  long	i,n;
	
  if(CdInit_f == ON)
   {	  
	ntoc = (short)CdGetToc(loc);
	to10();

	for (i=2;i<=ntoc;i++)
	 {
	  n = loc[i].sector;

	  n -= MINUS_SEEK;
	  if (n < 0)
	   {
		if (loc[i].second == 0)
		 {
		  loc[i].minute--;
		  loc[i].second = 59;
		 }
		else
		 {
		  loc[i].second--;
		 }
		n += 75;
	   }
	  loc[i].sector = n;
	 }
	to16();
	CdInit_f = OFF;
	
	return;
   }

  switch(CdJob)
   {
	case PLAY :
	    	   cd_play_main();
	    	   break;
	case STOP :
	    	   cd_stop_main();
	    	   break;
	case PAUSE :
	    		cd_pause_main();
	    		break;
	case DEPAUSE :
	    		  cd_depause_main();
	    		  break;
   }
  CdSync(1,0);
 }


short cd_get_end(void)
 {
  u_char	result[8];
  return	CdPlay_f;

  #if 0
  return CdReady(1, result);
  if(CdReady(1, result) == CdlDataEnd) return 1;
  return 0;
  #endif
 }

void cd_set_fade(short fade)
 {
  fade_effect = fade;
 }


void cd_play(void)
 {
  CdJob = PLAY;
  CdPlay_f = 1;
 }

void cd_seek(short id)
 {
  if(id > ntoc)
   {
	current_DA = -1;
	return;
   }

  current_DA = id;

	/* 決め手 */
  CdControl(CdlSeekP, (u_char *)&loc[id] , (u_char *)&loc[id]);

/*    CdControlB(CdlSetloc, (u_char *)&loc[current_DA], 0);*/
  CdJob = SEEK;
	
  last_seek = id;
 }


void cd_set_vol()
 {
  cd_max( bgm_vol[last_seek], 0, bgm_vol[last_seek], 0 );
  cd_mix( bgm_vol[last_seek], 0, bgm_vol[last_seek], 0 );
 }


static void cd_play_main(void)
 {
  int		ret;
  u_char	result[8];
  short	at;
  #ifdef	CD_DEBUG
   int		stat, track, min, sec, vol;
  #endif

  if(current_DA == -1) return;

  if(CdPlay_f == 1)
   {		/* まだ再生していない？ */
   	cd_set_vol();
	CdControl(CdlPlay, 0, 0);
	CdPlay_f = 2;
	return;
   }

  if((ret = CdReady(1, result)) == CdlDataReady) 
   {
	bbb = result[0];
    #ifdef	CD_DEBUG
	 stat = result[0];
	 track = btoi(result[1]);
	 min = btoi(result[3]);
	 sec = btoi(result[4]);
	 vol = result[6] + result[7];
    #endif
   }
  else
   if(ret == CdlDataEnd)
    {		/* 曲終わり？ */
	 CdPlay_f = 0;
	}
   else
    {
	 CdPlay_f = 0x80|ret;
	}

  if(fade_effect != 0)
   {			/* フェードイン，アウト */
	at = (short)atv.val0 + fade_effect;
	if(at < 0) at = 0;
	if(at > max.val0) at = max.val0;
    atv.val0 = (unsigned char)at;
	
	at = (short)atv.val1 + fade_effect;
	if(at < 0) at = 0;
	if(at > max.val1) at = max.val1;
	atv.val1 = (unsigned char)at;
	
	at = (short)atv.val2 + fade_effect;
	if(at < 0) at = 0;
	if(at > max.val2) at = max.val2;
	atv.val2 = (unsigned char)at;

	at = (short)atv.val3 + fade_effect;
	if(at < 0) at = 0;
	if(at > max.val3) at = max.val3;
	atv.val3 = (unsigned char)at;

	CdMix(&atv);

	if((atv.val0 == 0) && (atv.val1 == 0)
	&& (atv.val2 == 0) && (atv.val3 == 0)) fade_effect = 0;

	if((atv.val0 == max.val0) && (atv.val1 == max.val1)
	&& (atv.val2 == max.val2) && (atv.val3 == max.val3)) fade_effect = 0;
   }

  #ifdef	CD_DEBUG
   FntPrint("cd location = %d %d (%d)\n", min, sec, CdSync(1, 0));
  #endif
 }

void cd_stop(void)
 {
  CdStop_f = 0;
  CdJob = STOP;
 }


static void cd_stop_main()
 {
  u_char	result[8];

  if(CdStop_f == 0)
   {		/* 中止処理を初めていない？ */
	CdControl(CdlStop, 0, 0);
	CdStop_f = 1;
   }
 }

void cd_pause(void)
 {
  CdPause_f = 0;
  CdJob = PAUSE;
 }

static void cd_pause_main()
 {
  u_char	result[8];

  if(CdPause_f == 0)
   {		/* 中断処理を初めていない？ */
	CdControl(CdlPause, 0, 0);
	CdPause_f = 1;
   }
 }


void cd_depause(void)
 {
  CdPause_f = 0;
  CdJob = DEPAUSE;
 }


static void cd_depause_main()
 {
  CdPlay_f = 1;
  CdJob = PLAY;
 }
