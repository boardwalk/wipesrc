int16_t InitScene(Object** prm, int16_t *prmCount, Skeleton* camPos, ConfigData *gameData);
void getPath(char* trackPath, char trackNo, const char* name);
void LoadCmpFiles( TIMlist *timPtr );
Texture* NewLoadTexture( intptr_t timPtr, int16_t translucent );
TIMlist *LoadCompressedTextureSequence( const char *fileName );
