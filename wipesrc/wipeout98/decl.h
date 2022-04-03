#ifndef DECL_H
#define DECL_H

int32_t ShellGetGameDirectory(char *pGameDirectory);
int32_t ShellGetDataDirectory(char *pDataDirectory);
int32_t ShellGetMovieDirectory(char *pGameDirectory);
int32_t ShellGetPrefs();
int32_t ShellSetPrefs();
void CDRestart();
void CDPlay(int16_t CurrTrackNo);
void CDStop();
void LoadVRam(const char *filename, char set_pal);
int32_t SJRNewTexture(const char *filename);
void reloadStartScr();
void reloadScr();

#endif
