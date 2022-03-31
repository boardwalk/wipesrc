typedef struct {
	int32_t	width;
	int32_t	height;
	int32_t	vram_xpos;
	int32_t	vram_ypos;
	char	*bitmap;
	char	*palette;
} bm;

void ContinueOTag(void);
void ReadPCX(const char *fname, bm *BMptr,char Flg);
char *LockPilot(void);
void UnlockPilot(void);
int32_t AverageZ4( int32_t sz0, int32_t sz1, int32_t sz2, int32_t sz3 );
int32_t AverageZ3( int32_t sz0, int32_t sz1, int32_t sz2 );
