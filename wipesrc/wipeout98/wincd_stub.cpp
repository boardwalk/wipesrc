short int   		CurrTrkNo;
short starttrack;
short endtrack;
char	thieving_git=0;

void CdInit()
{}

void CdSetVol(long newVol)
{}

void CDRestart(void)
{}

int CdControl(unsigned char Cmd, unsigned char *parm1, unsigned char  *parm2)
{ return 0; }

int CdGetToc2(int sess, struct CdlPos *table)
{ return 0; }
