#ifndef INIT_H
#define INIT_H

int16_t InitScene(Object** prm, int16_t *prmCount, Skeleton* camPos, ConfigData *gameData);
void getPath(char* trackPath, char trackNo, const char* name);
int16_t LoadCompressedTextureSequence(const char* fileName);

#endif
