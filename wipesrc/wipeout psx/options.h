#define		FADE_INACTIVE 	0
#define		FADE_ACTIVE		1
#define		FADE_COMPLETE 	2
#define		FADE_OUT		 	3


typedef struct 
{
	short	fadeCount ;
	char	fadeFlag ;
}FadeData ;


void InGameReset(int choice, ulong *, char, ConfigData*, int*);
void InGameConfirm (char *confirm, int choice, short xpad, char *lConfirm, ulong *) ; 
void DeSelectScreen (POLY_F4 *, DR_MODE *) ;
void ScreenFade (POLY_F4 *, DR_MODE *, FadeData *) ;
