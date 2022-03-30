struct cdlPos
{
	int32_t min;
	int32_t sec;
};

void CDRestart(void);
void CdSetVol(int32_t newVol);
int32_t numTracks(void);
void CDPlay(int16_t CurrTrackNo);
int32_t CdGetToc2(int32_t sess, struct CdlPos *table);
