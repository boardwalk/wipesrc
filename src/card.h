#ifndef CARD_H
#define CARD_H

#define EUROPE 0
#define US 1
#define JAPAN 0
#define DEBUG 0
char SaveCardFile(char*, int32_t, ConfigData*, int16_t);
char RenameFile(char*, int32_t, ConfigData*, int16_t);
void GetHeader(char*);
void MakeFileName(char*, char*, int32_t);
char CreateFile(char*);
char WriteFile(char*, char*, ConfigData*, int16_t);
void LoadCardFiles(SelectionData*, char[][9], int32_t);
char LoadCardData(char*, int32_t, ConfigData*, SelectionData*);
char RenameFile1(char*, char*, int32_t, ConfigData*, int16_t);
void InitCardEvents();
int32_t GetCardInfo(int32_t);
int32_t CheckCardEvents();
void ClearEventHW();
void ClearEventSW();
int32_t CardContentStatus();
void LoadCard(int32_t);
int32_t UnFormat(int32_t);
int32_t QuickUnFormat(int32_t);
int32_t CheckHWCardEvents();
void LoadCardTexture();

#endif
