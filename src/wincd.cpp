#include <stdint.h>

int16_t CurrTrkNo;
int16_t starttrack;
int16_t endtrack;
char	thieving_git=0;

void CdInit()
{}

void CdSetVol(int32_t newVol)
{}

void CDRestart()
{}

int32_t CdControl(uint8_t Cmd, uint8_t *parm1, uint8_t  *parm2)
{ return 0; }

int32_t CdGetToc2(int32_t sess, struct CdlPos *table)
{ return 0; }
