struct cdlPos
{
	long min;
	long sec;
};

void CDRestart(void);
void CdSetVol(long newVol);
int numTracks(void);
void CDPlay(short int CurrTrackNo);
int CdGetToc2(int sess, struct CdlPos *table);
