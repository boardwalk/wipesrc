short InitScene(Object** prm, short *prmCount, Skeleton* camPos, ConfigData *gameData);
void getPath(char* trackPath, char trackNo, char* name);
void LoadCmpFiles( TIMlist *timPtr );
Texture* NewLoadTexture( intptr_t timPtr, short translucent );
TIMlist *LoadCompressedTextureSequence( char *fileName );
