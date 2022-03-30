int32_t ShellGetGameDirectory(char *pGameDirectory);
int32_t ShellGetDataDirectory(char *pDataDirectory);
int32_t ShellGetMovieDirectory(char *pGameDirectory);
int32_t ShellGetPrefs(void);
int32_t ShellSetPrefs(void);
void CDRestart();
void CDPlay(int16_t CurrTrackNo);
void CDStop();
void LoadVRam(char *filename, char set_pal);
int32_t SJRNewTexture(char *filename);
void reloadStartScr(void);
void reloadScr(void);
