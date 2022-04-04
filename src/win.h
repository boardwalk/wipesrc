#ifndef WIN_H
#define WIN_H

int32_t JJSLoadSprites ();
int32_t randy();
void centreMouse();
void changeScreen();
int32_t	     LockBackBuffer();
void	    UnlockBackBuffer();
void setRenderBegin();
void setRenderEnd();
int32_t	     ProcessMessages();
void SetContext();
void sendList();
void	    JJSDrawPolyF3(POLY_F3 * p);
void	    JJSDrawPolyFT3(POLY_FT3 * p);
void	    JJSDrawPolyG3(POLY_G3 * p);
void	    JJSDrawPolyGT3(POLY_GT3 * p);
void	    JJSDrawPolyGT4(POLY_GT4 * p);
void	    JJSDrawPolyF4(POLY_F4 * p);
void	    JJSDrawPolyFT4(POLY_FT4 * p);
void	    JJSDrawPolyG4(POLY_G4 * p);
void	    CopyPal(char *palette, int16_t start, int16_t end);
void	    WinQuit();
int32_t JJSJoyConfig();
void	    BltClearToScreen();

#endif
