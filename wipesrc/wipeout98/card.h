#define	EUROPE	0
#define	US			1
#define	JAPAN		0
#define	DEBUG		0
char SaveCardFile (char *, int, ConfigData *, short) ;		  
char RenameFile (char *, int, ConfigData *, short) ;			  
void GetHeader (char *) ;
void MakeFileName (char *, char *, int) ;	
char CreateFile (char *) ;
char WriteFile (char *, char *, ConfigData *, short) ;
void LoadCardFiles (SelectionData *, char [][9], int) ;	
char LoadCardData (char *, int, ConfigData *, SelectionData *)	;
char RenameFile1 (char *, char *, int, ConfigData *, short) ;	  
void InitCardEvents () ;
long GetCardInfo (int) ;						  
long CheckCardEvents ()	;
void ClearEventHW () ;
void ClearEventSW () ;
long CardContentStatus () ;
void LoadCard (int) ;						  
long UnFormat (long) ;
long QuickUnFormat (long) ;
long CheckHWCardEvents () ;
void LoadCardTexture ()	;
