/*******************************************************************
 *
 *    DESCRIPTION:	  	NULL Functions for compile purposes
 *
 *    AUTHOR:			Andy Y
 *
 *    HISTORY:    
 *
 *******************************************************************/

/** include files **/
#include <sys\types.h>
#include <libcd.h>

/** local definitions **/

/* default settings */

/** external functions **/

/** external data **/

/** internal functions **/

/** public data **/

/** private data **/

/** public functions **/

/** private functions **/

#ifndef USE_ATI


void ExitCriticalSection(void)
{
return;
} 

int CdMix(CdlATV *a)
{
return(1);
}  

int	CdSync(int a, u_char *b)
{
return(0);
}
#endif //USE_ATI


int	CdControlF(u_char a, u_char *b)
{
return(0);
}  
#ifndef USE_ATI


int CdReady(int a, u_char *b)
{
	return(CdlDataReady);
}
#endif //USE_ATI

int CdRead(int a, u_long *b, int c)
{
	return(0);
}


int	CdReadSync(int a, int b)
{
	return(0);
}

#ifndef USE_ATI

/* Look up next file */
struct DIRENTRY *nextfile(struct DIRENTRY *dir)
{
	return(dir);
}

/* Look up first file */
struct DIRENTRY *firstfile(char *name, struct DIRENTRY *dir)
{
	return(dir);
	
}
#endif //USE_ATI


/* Initialise file system */
long format(char *fs)
{
	return 1;
}

int GetGraphType(void)
{
	return(0);
}

#ifndef USE_ATI

int GetDriveInfo16(void)
{
	return(0);
}

#endif //USE_ATI
void	FntPrint(char* fred)
{
	
}
