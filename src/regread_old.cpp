#include <windows.h>
#include <stdio.h>
#include "config.h"

int32_t JJSJoyConfig();
//#include <windowsx.h>
//#include <winbase.h>
//#include <stdio.h>

/****************************************************************************
 *
 *
 *
 ****************************************************************************/

#define _KEYBOARD 1
#define _JOYSTICK1 2
#define _JOYSTICK2 3
#define _MOUSE1 4
#define _MOUSE2 5

#define _DEVICE_OFF 0
#define _DEVICE_ON 1

extern uint16_t etc_DefaultKeyboardSet[];
extern volatile uint8_t etc_Joystick1Switch; /* id = 1 */
extern volatile uint8_t etc_Mouse1Switch; /* id = 1 */
extern volatile uint8_t etc_InputDeviceSelect;
extern char orig_controller;
extern uint16_t Keycons[];

/* Registry stuff */
#define WO_REGISTRY_KEY "SOFTWARE\\3D Rage Games\\WipeOut"
#define WO_DIRECTORY_VALUE_NAME "Game Directory"
#define WO_MOVIE_VALUE_NAME "Movie Directory"
#define WO_DATA_DIRECTORY_VALUE_NAME "Data Directory"
#define WO_RESOLUTION_VALUE_NAME "Resolution"
#define WO_DISTANCE_VALUE_NAME "Distance"
#define WO_TEXTURE_VALUE_NAME "Texture"
#define WO_KEYCODE_VALUE_NAME "Keycode"
#define WO_SKY_VALUE_NAME "Sky"
#define WO_CONTROL_VALUE_NAME "Controller"
/****************************************************************************
 *
 *
 *
 ****************************************************************************/
extern char Resolution, controller;
extern int32_t DrawDistance;
extern int16_t ft;
extern char Sky;
extern uint16_t NewOriginalKeyCodes[], OriginalKeyCodes[];

int32_t ShellGetGameDirectory(char* pGameDirectory) {
  HKEY hkeyOurKey;

  char szValueName[128];
  DWORD dwValueNameLength;
  DWORD dwValueType;
  byte abValueBuffer[MAX_PATH];
  DWORD dwValueBufferLength;

  DWORD index;

  LONG retval;

  /* Open out registry key */
  retval = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, // handle of root key
      WO_REGISTRY_KEY, // address of name of subkey to open
      0, // reserved
      KEY_QUERY_VALUE, // security access mask
      &hkeyOurKey // address of handle of open key
  );
  if (retval == ERROR_SUCCESS) {
    /* So far so good - enumerate it's values to find the game directory */
    retval = ERROR_SUCCESS;
    for (index = 0; retval == ERROR_SUCCESS; index++) {
      dwValueNameLength = sizeof(szValueName);
      dwValueBufferLength = sizeof(abValueBuffer);
      retval = RegEnumValue(
          hkeyOurKey, // handle of key to query
          index, // index of value to query
          szValueName, // address of buffer for value string
          &dwValueNameLength, // address for size of value buffer
          NULL, // reserved
          &dwValueType, // address of buffer for type code
          abValueBuffer, // address of buffer for value data
          &dwValueBufferLength // address for size of data buffer
      );
      if (retval == ERROR_SUCCESS) {
        if (dwValueType == REG_SZ) {
          if (strcmp(szValueName, WO_DIRECTORY_VALUE_NAME) == 0) {
            /* Found the game directory value */
            strcpy(pGameDirectory, (char*)abValueBuffer);
            break;
          }
        }
      }
    }
    RegCloseKey(hkeyOurKey);
  }

  return retval;
}

int32_t ShellGetMovieDirectory(char* pGameDirectory) {
  HKEY hkeyOurKey;

  char szValueName[128];
  DWORD dwValueNameLength;
  DWORD dwValueType;
  byte abValueBuffer[MAX_PATH];
  DWORD dwValueBufferLength;

  DWORD index;

  LONG retval;

  /* Open out registry key */
  retval = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, // handle of root key
      WO_REGISTRY_KEY, // address of name of subkey to open
      0, // reserved
      KEY_QUERY_VALUE, // security access mask
      &hkeyOurKey // address of handle of open key
  );
  if (retval == ERROR_SUCCESS) {
    /* So far so good - enumerate it's values to find the game directory */
    retval = ERROR_SUCCESS;
    for (index = 0; retval == ERROR_SUCCESS; index++) {
      dwValueNameLength = sizeof(szValueName);
      dwValueBufferLength = sizeof(abValueBuffer);
      retval = RegEnumValue(
          hkeyOurKey, // handle of key to query
          index, // index of value to query
          szValueName, // address of buffer for value string
          &dwValueNameLength, // address for size of value buffer
          NULL, // reserved
          &dwValueType, // address of buffer for type code
          abValueBuffer, // address of buffer for value data
          &dwValueBufferLength // address for size of data buffer
      );
      if (retval == ERROR_SUCCESS) {
        if (dwValueType == REG_SZ) {
          if (strcmp(szValueName, WO_MOVIE_VALUE_NAME) == 0) {
            /* Found the game directory value */
            strcpy(pGameDirectory, (char*)abValueBuffer);
            break;
          }
        }
      }
    }
    RegCloseKey(hkeyOurKey);
  }

  return retval;
}

/****************************************************************************
 *
 *
 *
 ****************************************************************************/

int32_t ShellGetDataDirectory(char* pDataDirectory) {
  HKEY hkeyOurKey;

  char szValueName[128];
  DWORD dwValueNameLength;
  DWORD dwValueType;
  byte abValueBuffer[MAX_PATH];
  DWORD dwValueBufferLength;

  DWORD index;

  LONG retval;

  /* Open out registry key */
  retval = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, // handle of root key
      WO_REGISTRY_KEY, // address of name of subkey to open
      0, // reserved
      KEY_QUERY_VALUE, // security access mask
      &hkeyOurKey // address of handle of open key
  );
  if (retval == ERROR_SUCCESS) {
    /* So far so good - enumerate it's values to find the game directory */
    retval = ERROR_SUCCESS;
    for (index = 0; retval == ERROR_SUCCESS; index++) {
      dwValueNameLength = sizeof(szValueName);
      dwValueBufferLength = sizeof(abValueBuffer);
      retval = RegEnumValue(
          hkeyOurKey, // handle of key to query
          index, // index of value to query
          szValueName, // address of buffer for value string
          &dwValueNameLength, // address for size of value buffer
          NULL, // reserved
          &dwValueType, // address of buffer for type code
          abValueBuffer, // address of buffer for value data
          &dwValueBufferLength // address for size of data buffer
      );
      if (retval == ERROR_SUCCESS) {
        if (dwValueType == REG_SZ) {
          if (strcmp(szValueName, WO_DATA_DIRECTORY_VALUE_NAME) == 0) {
            /* Found the game directory value */
            strcpy(pDataDirectory, (char*)abValueBuffer);
            break;
          }
        }
      }
    }
    RegCloseKey(hkeyOurKey);
  }

  return retval;
}

static char* reses[] = {
    "SMALL",
    "LOW",
    "MEDIUM",
    "HIGH"};

static char* dists[] = {
    "NEAR",
    "MEDIUM",
    "FAR"};

static char* texts[] = {
    "OFF",
    "ON"};

static char Res[20] = "HIGH";
static char Dist[20] = "FAR";
static char Text[20] = "OFF";
static char Skys[20] = "ON";
static char KeyBuff[250] = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";

static int32_t RealDist[] = {
    18000,
    38000,
    65535};

int32_t ShellGetPrefs() {
  HKEY hkeyOurKey;
  static char tbuff[20] = "1";
  char szValueName[128];
  DWORD dwValueNameLength;
  DWORD dwValueType;
  byte abValueBuffer[MAX_PATH];
  DWORD dwValueBufferLength;
  DWORD index;

  LONG retval;
  int32_t i;

  /* Open out registry key */
  retval = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, // handle of root key
      WO_REGISTRY_KEY, // address of name of subkey to open
      0, // reserved
      KEY_QUERY_VALUE, // security access mask
      &hkeyOurKey // address of handle of open key
  );
  if (retval == ERROR_SUCCESS) {
    /* So far so good - enumerate it's values to find the game directory */
    retval = ERROR_SUCCESS;
    for (index = 0; retval == ERROR_SUCCESS; index++) {
      dwValueNameLength = sizeof(szValueName);
      dwValueBufferLength = sizeof(abValueBuffer);
      retval = RegEnumValue(
          hkeyOurKey, // handle of key to query
          index, // index of value to query
          szValueName, // address of buffer for value string
          &dwValueNameLength, // address for size of value buffer
          NULL, // reserved
          &dwValueType, // address of buffer for type code
          abValueBuffer, // address of buffer for value data
          &dwValueBufferLength // address for size of data buffer
      );
      if (retval == ERROR_SUCCESS) {
        if (dwValueType == REG_SZ) {
          if (strcmp(szValueName, WO_RESOLUTION_VALUE_NAME) == 0)
            strcpy(Res, (char*)abValueBuffer);
          if (strcmp(szValueName, WO_DISTANCE_VALUE_NAME) == 0)
            strcpy(Dist, (char*)abValueBuffer);
          if (strcmp(szValueName, WO_TEXTURE_VALUE_NAME) == 0)
            strcpy(Text, (char*)abValueBuffer);
          if (strcmp(szValueName, WO_KEYCODE_VALUE_NAME) == 0)
            strcpy(KeyBuff, (char*)abValueBuffer);
          if (strcmp(szValueName, WO_CONTROL_VALUE_NAME) == 0)
            strcpy(tbuff, (char*)abValueBuffer);
          if (strcmp(szValueName, WO_SKY_VALUE_NAME) == 0)
            strcpy(Skys, (char*)abValueBuffer);
        }
      }
    }
    RegCloseKey(hkeyOurKey);
  }
  Resolution = 3;
  for (i = 0; i < 4; ++i) {
    if (!strcmp(reses[i], Res)) {
      Resolution = i;
      break;
    }
  }
  ft = 0;
  for (i = 0; i < 2; ++i) {
    if (!strcmp(texts[i], Text)) {
      ft = i;
      break;
    }
  }
  Sky = 1;
  for (i = 0; i < 2; ++i) {
    if (!strcmp(texts[i], Skys)) {
      Sky = i;
      break;
    }
  }
  DrawDistance = 65535;
  for (i = 0; i < 3; ++i) {
    if (!strcmp(dists[i], Dist)) {
      DrawDistance = RealDist[i];
      break;
    }
  }
  sscanf(KeyBuff, "%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd,%hd",
         &NewOriginalKeyCodes[0],
         &NewOriginalKeyCodes[1],
         &NewOriginalKeyCodes[4],
         &NewOriginalKeyCodes[5],
         &NewOriginalKeyCodes[6],
         &NewOriginalKeyCodes[11],
         &NewOriginalKeyCodes[12],
         &NewOriginalKeyCodes[13],
         &NewOriginalKeyCodes[14],
         &NewOriginalKeyCodes[15]);
  if (NewOriginalKeyCodes[0])
    for (i = 0; i < 16; ++i) {
      if (NewOriginalKeyCodes[i]) {
        OriginalKeyCodes[i] = etc_DefaultKeyboardSet[i] = NewOriginalKeyCodes[i];
        SetKey(i, NewOriginalKeyCodes[i]);
      }
    }
  // Keep a copy of the keys
  Keycons[0] = OriginalKeyCodes[15];
  Keycons[1] = OriginalKeyCodes[13];
  Keycons[2] = OriginalKeyCodes[12];
  Keycons[3] = OriginalKeyCodes[14];
  Keycons[4] = OriginalKeyCodes[0];
  Keycons[5] = OriginalKeyCodes[1];
  Keycons[6] = OriginalKeyCodes[6];
  Keycons[7] = OriginalKeyCodes[5];
  Keycons[8] = OriginalKeyCodes[4];
  Keycons[9] = OriginalKeyCodes[11];
  controller = atoi(tbuff);
  switch (controller) {
  case _JOYSTICK1:
    if (JJSJoyConfig()) {
      etc_Joystick1Switch = _DEVICE_ON;
      etc_InputDeviceSelect = _JOYSTICK1;
    } else {
      etc_Joystick1Switch = _DEVICE_OFF;
      etc_InputDeviceSelect = _KEYBOARD;
      orig_controller = _KEYBOARD;
    }
    break;
  case _MOUSE1:
    etc_Mouse1Switch = _DEVICE_ON;
    etc_InputDeviceSelect = _MOUSE1;
    break;
  default:
    break;
  }
  return retval;
}

int32_t ShellSetPrefs() {
  HKEY hkeyOurKey;

  char szValueName[128], tbuff[20];
  DWORD dwValueNameLength;
  DWORD dwValueType;
  byte abValueBuffer[MAX_PATH];
  DWORD dwValueBufferLength;
  DWORD index;

  LONG retval;

  sprintf(tbuff, "%d", controller);
  sprintf(KeyBuff, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
          NewOriginalKeyCodes[0],
          NewOriginalKeyCodes[1],
          NewOriginalKeyCodes[4],
          NewOriginalKeyCodes[5],
          NewOriginalKeyCodes[6],
          NewOriginalKeyCodes[11],
          NewOriginalKeyCodes[12],
          NewOriginalKeyCodes[13],
          NewOriginalKeyCodes[14],
          NewOriginalKeyCodes[15]);
  /* Open out registry key */
  retval = RegOpenKeyEx(
      HKEY_LOCAL_MACHINE, // handle of root key
      WO_REGISTRY_KEY, // address of name of subkey to open
      0, // reserved
      KEY_QUERY_VALUE, // security access mask
      &hkeyOurKey // address of handle of open key
  );
  strcpy(Res, reses[Resolution]);
  switch (DrawDistance) {
  case 18000:
    strcpy(Dist, dists[0]);
    break;
  case 38000:
    strcpy(Dist, dists[1]);
    break;
  case 65535:
    strcpy(Dist, dists[2]);
    break;
  }
  strcpy(Text, texts[ft]);
  strcpy(Skys, texts[Sky]);
  if (retval == ERROR_SUCCESS) {
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_RESOLUTION_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)Res, // Address of data
        strlen(Res) + 1 // Size of data
    );
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_DISTANCE_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)Dist, // Address of data
        strlen(Dist) + 1 // Size of data
    );
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_TEXTURE_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)Text, // Address of data
        strlen(Text) + 1 // Size of data
    );
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_SKY_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)Skys, // Address of data
        strlen(Skys) + 1 // Size of data
    );
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_KEYCODE_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)KeyBuff, // Address of data
        strlen(KeyBuff) + 1 // Size of data
    );
    retval = RegSetValueEx(
        hkeyOurKey, // The key
        WO_CONTROL_VALUE_NAME, // Name of our value
        0, // Reserved
        REG_SZ, // Data type
        (CONST BYTE*)tbuff, // Address of data
        strlen(tbuff) + 1 // Size of data
    );
  }

  RegCloseKey(hkeyOurKey);
  return retval;
}
