/************
*
* CWDDE Header information
*
*************/

#ifndef CWDDE_H
#define CWDDE_H

#define Control_ConfigIsSupported   0x7300
#define Control_ConfigIsEnabled     0x7301
#define Control_GetConfiguration    0x7302
#define Control_EngSemIsSupported   0x7320
#define Control_EngSemIsEnabled     0x7321
#define Control_GetSemaphorePtr     0x7322
#define Control_EngineShareBegin    0x7323
#define Control_EngineShareEnd      0x7324
#define Control_TimewarpIsSupported 0x7340
#define Control_TimewarpIsEnabled   0x7341
#define Control_TimewarpEnable      0x7342
#define Control_TimewarpDisable     0x7343

typedef struct tagPTR48
  {
    DWORD Offset;
    WORD  Selector;
    WORD  Reserved;
  } PTR48;

typedef PTR48 FAR *LPPTR48;

typedef struct tagATICONFIG
  {
    BYTE ATISig[10];
    BYTE Filler1[2];
    BYTE DriverName[9];
    BYTE Filler2[3];
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwDesktopWidth;
    DWORD dwDesktopHeight;
    DWORD dwEnginePitch;
    DWORD dwRealRamAvailable;
    DWORD dwVGABoundary;
    DWORD dwBpp;
    DWORD dwBoardBpp;
    DWORD dwColorFormat;
    DWORD dwAlphaBitMask;
    DWORD dwConfigBits;
    DWORD dwAsicRevision;
    DWORD dwRomVersion;
    DWORD dwBoardType;
    DWORD dwApertureType;
    PTR48 AperturePtr;
    DWORD DisplayOffset;
    PTR48 MemRegPtr;
  } ATICONFIG;

#define DWORD_FAIL 0xFFFFFFFF

#endif /* CWDDE_H */

#ifdef __cplusplus
extern "C" {
#endif

WORD CWDDE_Init ( void );
WORD CWDDE_Free ( void );
DWORD CWDDE_GetBoardType ( void );
BOOL CWDDE_GetATISig(char *sig);
BOOL CWDDE_GetDriverName(char *dname);
DWORD CWDDE_GetDesktopWidth(void);
DWORD CWDDE_GetDesktopHeight(void);
DWORD CWDDE_GetBoardBpp(void);
DWORD CWDDE_GetRam (void);
DWORD CWDDE_GetAperatureType (void);
DWORD CWDDE_GetMajorVersion(void);
DWORD CWDDE_GetMinorVersion(void);
DWORD CWDDE_GetBuildNumber(void);
WORD CWDDE_GetMemRegPtr ( LPPTR48 lpPtr48 );
WORD CWDDE_GetScreenPtr ( LPPTR48 lpPtr48 );
DWORD CWDDE_GetDisplayOffset ( void );

#ifdef __cplusplus
}
#endif

/************
*
* CWDDE Header information
*
*************/

#ifndef CWDDE_H
#define CWDDE_H

#define Control_ConfigIsSupported   0x7300
#define Control_ConfigIsEnabled     0x7301
#define Control_GetConfiguration    0x7302
#define Control_EngSemIsSupported   0x7320
#define Control_EngSemIsEnabled     0x7321
#define Control_GetSemaphorePtr     0x7322
#define Control_EngineShareBegin    0x7323
#define Control_EngineShareEnd      0x7324
#define Control_TimewarpIsSupported 0x7340
#define Control_TimewarpIsEnabled   0x7341
#define Control_TimewarpEnable      0x7342
#define Control_TimewarpDisable     0x7343

typedef struct tagPTR48
  {
    DWORD Offset;
    WORD  Selector;
    WORD  Reserved;
  } PTR48;

typedef PTR48 FAR *LPPTR48;

typedef struct tagATICONFIG
  {
    BYTE ATISig[10];
    BYTE Filler1[2];
    BYTE DriverName[9];
    BYTE Filler2[3];
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwDesktopWidth;
    DWORD dwDesktopHeight;
    DWORD dwEnginePitch;
    DWORD dwRealRamAvailable;
    DWORD dwVGABoundary;
    DWORD dwBpp;
    DWORD dwBoardBpp;
    DWORD dwColorFormat;
    DWORD dwAlphaBitMask;
    DWORD dwConfigBits;
    DWORD dwAsicRevision;
    DWORD dwRomVersion;
    DWORD dwBoardType;
    DWORD dwApertureType;
    PTR48 AperturePtr;
    DWORD DisplayOffset;
    PTR48 MemRegPtr;
  } ATICONFIG;

#define DWORD_FAIL 0xFFFFFFFF

#endif /* CWDDE_H */

#ifdef __cplusplus
extern "C" {
#endif

WORD CWDDE_Init ( void );
WORD CWDDE_Free ( void );
DWORD CWDDE_GetBoardType ( void );
BOOL CWDDE_GetATISig(char *sig);
BOOL CWDDE_GetDriverName(char *dname);
DWORD CWDDE_GetDesktopWidth(void);
DWORD CWDDE_GetDesktopHeight(void);
DWORD CWDDE_GetBoardBpp(void);
DWORD CWDDE_GetRam (void);
DWORD CWDDE_GetAperatureType (void);
DWORD CWDDE_GetMajorVersion(void);
DWORD CWDDE_GetMinorVersion(void);
DWORD CWDDE_GetBuildNumber(void);
WORD CWDDE_GetMemRegPtr ( LPPTR48 lpPtr48 );
WORD CWDDE_GetScreenPtr ( LPPTR48 lpPtr48 );
DWORD CWDDE_GetDisplayOffset ( void );

#ifdef __cplusplus
}
#endif

/************
*
* CWDDE Header information
*
*************/

#ifndef CWDDE_H
#define CWDDE_H

#define Control_ConfigIsSupported   0x7300
#define Control_ConfigIsEnabled     0x7301
#define Control_GetConfiguration    0x7302
#define Control_EngSemIsSupported   0x7320
#define Control_EngSemIsEnabled     0x7321
#define Control_GetSemaphorePtr     0x7322
#define Control_EngineShareBegin    0x7323
#define Control_EngineShareEnd      0x7324
#define Control_TimewarpIsSupported 0x7340
#define Control_TimewarpIsEnabled   0x7341
#define Control_TimewarpEnable      0x7342
#define Control_TimewarpDisable     0x7343

typedef struct tagPTR48
  {
    DWORD Offset;
    WORD  Selector;
    WORD  Reserved;
  } PTR48;

typedef PTR48 FAR *LPPTR48;

typedef struct tagATICONFIG
  {
    BYTE ATISig[10];
    BYTE Filler1[2];
    BYTE DriverName[9];
    BYTE Filler2[3];
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwDesktopWidth;
    DWORD dwDesktopHeight;
    DWORD dwEnginePitch;
    DWORD dwRealRamAvailable;
    DWORD dwVGABoundary;
    DWORD dwBpp;
    DWORD dwBoardBpp;
    DWORD dwColorFormat;
    DWORD dwAlphaBitMask;
    DWORD dwConfigBits;
    DWORD dwAsicRevision;
    DWORD dwRomVersion;
    DWORD dwBoardType;
    DWORD dwApertureType;
    PTR48 AperturePtr;
    DWORD DisplayOffset;
    PTR48 MemRegPtr;
  } ATICONFIG;

#define DWORD_FAIL 0xFFFFFFFF

#endif /* CWDDE_H */

#ifdef __cplusplus
extern "C" {
#endif

WORD CWDDE_Init ( void );
WORD CWDDE_Free ( void );
DWORD CWDDE_GetBoardType ( void );
BOOL CWDDE_GetATISig(char *sig);
BOOL CWDDE_GetDriverName(char *dname);
DWORD CWDDE_GetDesktopWidth(void);
DWORD CWDDE_GetDesktopHeight(void);
DWORD CWDDE_GetBoardBpp(void);
DWORD CWDDE_GetRam (void);
DWORD CWDDE_GetAperatureType (void);
DWORD CWDDE_GetMajorVersion(void);
DWORD CWDDE_GetMinorVersion(void);
DWORD CWDDE_GetBuildNumber(void);
WORD CWDDE_GetMemRegPtr ( LPPTR48 lpPtr48 );
WORD CWDDE_GetScreenPtr ( LPPTR48 lpPtr48 );
DWORD CWDDE_GetDisplayOffset ( void );

#ifdef __cplusplus
}
#endif

/************
*
* CWDDE Header information
*
*************/

#ifndef CWDDE_H
#define CWDDE_H

#define Control_ConfigIsSupported   0x7300
#define Control_ConfigIsEnabled     0x7301
#define Control_GetConfiguration    0x7302
#define Control_EngSemIsSupported   0x7320
#define Control_EngSemIsEnabled     0x7321
#define Control_GetSemaphorePtr     0x7322
#define Control_EngineShareBegin    0x7323
#define Control_EngineShareEnd      0x7324
#define Control_TimewarpIsSupported 0x7340
#define Control_TimewarpIsEnabled   0x7341
#define Control_TimewarpEnable      0x7342
#define Control_TimewarpDisable     0x7343

typedef struct tagPTR48
  {
    DWORD Offset;
    WORD  Selector;
    WORD  Reserved;
  } PTR48;

typedef PTR48 FAR *LPPTR48;

typedef struct tagATICONFIG
  {
    BYTE ATISig[10];
    BYTE Filler1[2];
    BYTE DriverName[9];
    BYTE Filler2[3];
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwDesktopWidth;
    DWORD dwDesktopHeight;
    DWORD dwEnginePitch;
    DWORD dwRealRamAvailable;
    DWORD dwVGABoundary;
    DWORD dwBpp;
    DWORD dwBoardBpp;
    DWORD dwColorFormat;
    DWORD dwAlphaBitMask;
    DWORD dwConfigBits;
    DWORD dwAsicRevision;
    DWORD dwRomVersion;
    DWORD dwBoardType;
    DWORD dwApertureType;
    PTR48 AperturePtr;
    DWORD DisplayOffset;
    PTR48 MemRegPtr;
  } ATICONFIG;

#define DWORD_FAIL 0xFFFFFFFF

#endif /* CWDDE_H */

#ifdef __cplusplus
extern "C" {
#endif

WORD CWDDE_Init ( void );
WORD CWDDE_Free ( void );
DWORD CWDDE_GetBoardType ( void );
BOOL CWDDE_GetATISig(char *sig);
BOOL CWDDE_GetDriverName(char *dname);
DWORD CWDDE_GetDesktopWidth(void);
DWORD CWDDE_GetDesktopHeight(void);
DWORD CWDDE_GetBoardBpp(void);
DWORD CWDDE_GetRam (void);
DWORD CWDDE_GetAperatureType (void);
DWORD CWDDE_GetMajorVersion(void);
DWORD CWDDE_GetMinorVersion(void);
DWORD CWDDE_GetBuildNumber(void);
WORD CWDDE_GetMemRegPtr ( LPPTR48 lpPtr48 );
WORD CWDDE_GetScreenPtr ( LPPTR48 lpPtr48 );
DWORD CWDDE_GetDisplayOffset ( void );

#ifdef __cplusplus
}
#endif
