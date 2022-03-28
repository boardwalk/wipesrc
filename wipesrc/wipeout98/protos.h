typedef struct {
	int	width;
	int	height;
	int	vram_xpos;
	int	vram_ypos;
	char	*bitmap;
	char	*palette;
} bm;

void ContinueOTag(void);
void ReadPCX(char *fname, bm *BMptr,char Flg);
char *LockPilot(void);
void UnlockPilot(void);
long AverageZ4( long sz0, long sz1, long sz2, long sz3 );
long AverageZ3( long sz0, long sz1, long sz2 );
