short InitScene(Object** prm, short *prmCount, Skeleton* camPos, ConfigData *gameData);
void getPath(char* trackPath, char trackNo, char* name);
void LoadCmpFiles( TIMlist *timPtr );
Texture* NewLoadTexture( long timPtr, short translucent );
TIMlist *LoadCompressedTextureSequence( char *fileName );
