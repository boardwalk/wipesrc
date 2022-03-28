/***************************************************************************
 * 
 * Assault Rigs 3D Rage Uninstaller
 *
 ***************************************************************************/
 
#include <windows.h>
//#include <shellapi.h>   // for SHFileOperation
//#include <shlobj.h>     // for SHBroweForFolder
#include <regstr.h>
#include "dinstall.h"
#include "resource.h"

/* */
#define APP_TITLE "'Assault Rigs for 3D Rage'"

/* */
#define REGISTRY_KEY_GROUP	  		  "SOFTWARE\\3D Rage Games"
#define REGISTRY_KEY 		  		  "SOFTWARE\\3D Rage Games\\Assault Rigs"
#define REGISTRY_DIRECTORY_VALUE_NAME "Game Directory"

/* */
#define UNINSTALL_KEY_NAME		"Assault Rigs for 3D Rage"
#define UNINSTALL_DISPLAY_NAME	"Assault Rigs for 3D Rage"
#define UNINSTALL_APP			"UNARIGS.EXE"

#define GAME_APP "ARIGS.EXE"
#define HELP_APP "ARIGS.HLP"

#define SAVEGAME_NAME "SAVEGAME.DAT"

/* */
#define DIALOG_CONFIRM 	0
#define DIALOG_FINAL	1

/* List of uninstall files */
static char *pszUninstallFiles[] =
{
	UNINSTALL_APP,
	NULL	
};

/* List of savegame files */
static char *pszSaveGameFiles[] =
{
    SAVEGAME_NAME,
	NULL	
};

/* List of standard files */
static char *pszStandardFiles[] =
{
//    SAVEGAME_NAME,
	NULL	
};

/* List of game data files */
static char *pszDataFiles[] =
{
	"ARIGS.EXE",
	"ARIGS.DOC",
	"README.TXT",
	"RES.RDR",
	"RES.RDA",
	"RES.RDT",
	NULL	
};

/* List of movie files */
static char *pszMovieFiles[] =
{
	"LOGO.MPG",
	"INTRO.MPG",
	"OVER1.MPG",
	"OVER2.MPG",
	"OVER3.MPG",
	"OVER4.MPG",
	"VICTORY.MPG",
	NULL
};

/* The name of the executable on the CD */
char szExeName[] = GAME_APP;
char szHelpName[] = HELP_APP;
char szUninstallName[] = UNINSTALL_APP;

/* Registry key path */
char szRegistryKeyPath[] = REGISTRY_KEY;

/* 
 *
 */

static int gbDeleteOldSaveGame = FALSE;

/*
 * title of game installation
 */
static char title[] = "Assault Rigs for 3D Rage Uninstaller";

/*
 * title of program group
 */
static char group_name[] = "3D Rage Games";
static char exe_link_name[] = "Assault Rigs";
static char hlp_link_name[] = "Assault Rigs Help";
static char uninstall_link_name[] = "Uninstall Assault Rigs";

/*
 * prototypes
 */
BOOL FAR PASCAL masterDlgProc( HWND hdlg,DWORD message,DWORD wparam,DWORD lparam );

/*
 * globals
 */
static HANDLE hinst;

static char PreviousGameDirectory[MAX_PATH];	// Location of any previous install

/* */
void SetDialogText(HWND hDlg, DWORD id, char *sz, ...)
{
    char ach[256];
    wvsprintf(ach, sz, (void*)(&sz+1));
    SetDlgItemText(hDlg, id, ach);
}

/*
 * support functions
 */
void catpath(char *dst, char *src)
{
    int len = lstrlen(dst);
    if (len > 0 && (dst[len-1] != '\\' && dst[len-1] != '/'))
        lstrcat(dst,"\\");
    lstrcat(dst,src);

    // SHFileOperation needs a double null string.
    len = lstrlen(dst);
    dst[len+1] = 0;
}

/*
 * set a bitmap into a static control
 */
void SetBitmap(HWND hDlg, int id, char *szBitmap, int w, int h)
{
    HBITMAP hbm;
    HWND hwnd;
	RECT rect;

    hwnd = GetDlgItem(hDlg, id);

    if (hwnd == NULL)
        return;

	/* Get the actual size of our bitmap area so we can stretch to fit */
	GetClientRect(hwnd, &rect);

    hbm = (HBITMAP)LoadImage(hinst, szBitmap, IMAGE_BITMAP, 
        rect.right - rect.left, rect.bottom - rect.top,
        LR_LOADMAP3DCOLORS | LR_CREATEDIBSECTION);
    
    if (hbm)
        hbm = (HBITMAP)SendMessage(hwnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hbm);

    if (hbm)
        DeleteObject(hbm);
}

int DeleteKey(HKEY hkeyParentKey, char *szKeyPath)
{
	HKEY hkey;
	DWORD dwDisposition;

    char szClass[128];
    DWORD  cchClass;
    DWORD  cSubKeys;
    DWORD  cchMaxSubkey;
    DWORD  cchMaxClass;
    DWORD  cValues;
    DWORD  cchMaxValueName;
    DWORD  cbMaxValueData;
    DWORD  cbSecurityDescriptor;
    FILETIME  ftLastWriteTime;

	LONG retval;

	/* Check the key for sub-keys, if it has any we shouldn't delete it */
	/* (and can't under NT ) 											*/
	retval = RegCreateKeyEx(hkeyParentKey, 				// Parent key
							szKeyPath, 					// Name of new key
							0, 							// Reserved
							NULL, 						// Class string
							REG_OPTION_NON_VOLATILE, 	// Options
							KEY_QUERY_VALUE,			// Security access
							NULL,						// Pointer to security stuff
							&hkey,						// Our key result
							&dwDisposition);			// Disposition result
	if (retval == ERROR_SUCCESS)
	{
		/* Query key info */
		cchClass = sizeof(szClass);
	 	retval = RegQueryInfoKey(
			hkey,					// handle of key to query 
		    szClass,				// address of buffer for class string 
		    &cchClass,				// address of size of class string buffer 
		    NULL,					// reserved 
		    &cSubKeys,				// address of buffer for number of subkeys 
		    &cchMaxSubkey,			// address of buffer for longest subkey name length  
		    &cchMaxClass,			// address of buffer for longest class string length 
		    &cValues,				// address of buffer for number of value entries 
		    &cchMaxValueName,		// address of buffer for longest value name length 
		    &cbMaxValueData,		// address of buffer for longest value data length 
		    &cbSecurityDescriptor,	// address of buffer for security descriptor length 
		    &ftLastWriteTime 		// address of buffer for last write time 
	   );	

		/* Close key win or lose */
		RegCloseKey(hkey);

		if (retval == ERROR_SUCCESS)
		{
			/* If the key has no sub-keys we can delete it */
			if (cSubKeys == 0)
			{
				retval = RegDeleteKey(hkeyParentKey, szKeyPath);
			}
		}
	}

	/* Translate return code */
	if (retval == ERROR_SUCCESS)
	{
		return(0);
	}
	else
	{
		return(-1);
	}
}	

int DeleteRegistryEntries(void)
{
	char szKeyPath[MAX_PATH];
	int result;

	/* Delete the application's key and all its values */
	result = DeleteKey(HKEY_LOCAL_MACHINE, REGISTRY_KEY);

	if (result == 0)
	{
		/* Delete the application group iff it is now empty */
		result = DeleteKey(HKEY_LOCAL_MACHINE, REGISTRY_KEY_GROUP);
	}

	if (result == 0)
	{
		/* Delete the uninstall information */
		strcpy(szKeyPath, REGSTR_PATH_UNINSTALL);
		catpath(szKeyPath, UNINSTALL_KEY_NAME);
		result = DeleteKey(HKEY_LOCAL_MACHINE, szKeyPath);
	}

	return(result);
}	

int DeleteAFile(char *szPathName)
{
	DWORD dFileAttributes;
	BOOL bRetVal;
	int result;

	/* */
	result = 0;

	/* If the file exists */
	dFileAttributes = GetFileAttributes(szPathName);
	if (dFileAttributes != 0xffffffff)
	{
		/* Delete it */
    	bRetVal = DeleteFile(szPathName);
		if (bRetVal != TRUE)    
		{
			/* Failed to delete file */
			result = -1;
		}
	}

	return(result);
}	

int DeleteShortcut(char *szLinkName)
{
	char szLinkPathName[MAX_PATH];
	int result;
	
	/* Build path name for link in start up menu */
    GetWindowsDirectory(szLinkPathName, sizeof(szLinkPathName));
	catpath(szLinkPathName, "Start Menu\\Programs\\3D Rage Games");
	catpath(szLinkPathName, szLinkName);
	strcat(szLinkPathName, ".LNK");

	/* Delete the link file */
	result = DeleteAFile(szLinkPathName);

	return(result);
}	

int DeleteShortcuts(void)
{
	char szLinkGroupPathName[MAX_PATH];

	int result;
	
	result = DeleteShortcut(uninstall_link_name);
	if (result == 0)
	{
		result = DeleteShortcut(exe_link_name);
		if (result == 0)
		{
			result = DeleteShortcut(hlp_link_name);
			if (result == 0)
			{
				/* Delete the shortcut group if it is empty */

				/* Build path name for link group in start up menu */
			    GetWindowsDirectory(szLinkGroupPathName, sizeof(szLinkGroupPathName));
				catpath(szLinkGroupPathName, "Start Menu\\Programs\\3D Rage Games");

				/* The directory will only be removed if it is empty */
				RemoveDirectory(szLinkGroupPathName);
			}
		}
	}
	return(result);
}	

int DeleteFiles(HWND hDlg, char **pszList, char *szDir)
{
	char **pszNext;
	char *szName;

    char szPathName[MAX_PATH];

	DWORD dFileAttributes;
	BOOL bRetVal;

    int result;

	pszNext = pszList;
	result = 0;
	while (*pszNext && (result == 0))
	{
		szName = *pszNext++;

        lstrcpy(szPathName, szDir);
        catpath(szPathName, szName);

		/* If the file exists */
		dFileAttributes = GetFileAttributes(szPathName);
		if (dFileAttributes != 0xffffffff)
		{
			///* Delete it */
			//SetDialogText(hDlg, IDC_ACTION, "Deleting %s", szName);

	    	bRetVal = DeleteFile(szPathName);
			if (bRetVal != TRUE)    
			{
				/* Failed to delete file */
				result = -1;
				break;
			}
		}
	}

	return(result);
}	

int GetExistingGameDirectory(char *szGameDirectory)
{
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
    	HKEY_LOCAL_MACHINE,				// handle of root key
    	REGISTRY_KEY,					// address of name of subkey to open 
    	0,								// reserved 
    	KEY_QUERY_VALUE,				// security access mask 
    	&hkeyOurKey						// address of handle of open key 
	);	
	if (retval == ERROR_SUCCESS)
	{
		/* So far so good - enumerate it's values to find the game directory */
		retval = ERROR_SUCCESS;
		for (index = 0; retval == ERROR_SUCCESS; index++)
		{
			dwValueNameLength = sizeof(szValueName);
			dwValueBufferLength = sizeof(abValueBuffer);
			retval = RegEnumValue(
			    hkeyOurKey,				// handle of key to query 
			  	index,					// index of value to query 
	    		szValueName,			// address of buffer for value string 
		    	&dwValueNameLength,		// address for size of value buffer 
	  	  		NULL,					// reserved 
		    	&dwValueType,			// address of buffer for type code 
		    	abValueBuffer,			// address of buffer for value data 
		    	&dwValueBufferLength	// address for size of data buffer 
	   		);
			if (retval == ERROR_SUCCESS)
			{
				if (dwValueType == REG_SZ)
				{
					if (strcmp(szValueName, REGISTRY_DIRECTORY_VALUE_NAME) == 0)
					{
						/* Found the game directory value */
						strcpy(szGameDirectory, (char *)abValueBuffer);
						break;
					}
				}
			}
		}
		RegCloseKey(hkeyOurKey);
	}

	return(retval);
}	

/*
 * dlg proc for wizard dialog box, the setup is controlled from here.
 */
BOOL FAR PASCAL masterDlgProc(HWND hDlg,DWORD dwMessage,DWORD wParam,DWORD lParam)
{
    int         result;
    static int  busy;

	RECT rc;
	int w,h;

    switch(dwMessage)
    {
    case WM_INITDIALOG:
        busy = 0;
		busy++;
    
      	SetWindowText(hDlg, title);

		/* Set our bitmap */
		SetBitmap(hDlg, IDC_STATIC, "rigs", 160, 120);

		/* */
		EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
		EnableWindow(GetDlgItem(hDlg, IDC_CANCEL), FALSE);

		/* Centre dialog */
		GetWindowRect(GetDesktopWindow(),&rc);
		w = rc.right - rc.left;
		h = rc.bottom - rc.top;
		GetWindowRect(hDlg, &rc);
		w -= (rc.right - rc.left);
		h -= (rc.bottom - rc.top);
		SetWindowPos(
			hDlg, 
		    HWND_NOTOPMOST,
		    (w / 2),
		    (h / 2),
		    0,
		    0,
		    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
		);
		
		/* Look for a previous installation */
		result = GetExistingGameDirectory(PreviousGameDirectory);
		if (result != ERROR_SUCCESS)
		{
            result = MessageBox(
	            hDlg, 
	            APP_TITLE " is not currently installed", 
	            title, 
	            MB_OK
            );
			/* Abort uninstall */
            EndDialog(hDlg, 0);
		}

		/* */
		EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), TRUE);
		EnableWindow(GetDlgItem(hDlg, IDC_CANCEL), TRUE);

		/* Set focus to the CANCEL option */
		SetFocus(GetDlgItem(hDlg, IDC_CANCEL));

		busy--;

        /*
         * return 0 here indicating we have set the focus for the dialog box
         * and it doesn't need to help us
         */
        return 0;

    case WM_SETCURSOR:
        if (busy)
        {
            SetCursor(LoadCursor(NULL, IDC_WAIT));
            return TRUE;
        }
        break;

    case WM_COMMAND:
        switch(wParam)
        {
	        case IDC_CANCEL:
	            if( !busy )
	            {
	                /*
	                 * only allow cancel if we aren't doing anything else
	                 */
	                EndDialog( hDlg, -1 );
	            }
	            break;
	        
	        case IDC_UNINSTALL:
	            busy++;

				/* */
				EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), FALSE);
				EnableWindow(GetDlgItem(hDlg, IDC_CANCEL), FALSE);

	            result = MessageBox(hDlg, "Do you want to keep your saved game files?", title, MB_YESNOCANCEL | MB_DEFBUTTON1);
				if (result == IDCANCEL)
				{
					/* Cancel */				
					EnableWindow(GetDlgItem(hDlg, IDC_UNINSTALL), TRUE);
					EnableWindow(GetDlgItem(hDlg, IDC_CANCEL), TRUE);
					busy--;
					break;
				}
				else
				{
					if (result == IDYES)
					{
						gbDeleteOldSaveGame = FALSE;
					}
					else
					{
						gbDeleteOldSaveGame = TRUE;
					}

					/* */
					//SetDialogText(hDlg, IDC_ACTION, "Deleting StartMenu shortcuts");
					result = DeleteShortcuts();
					//SetDialogText(hDlg, IDC_ACTION, "");
					if (result == 0)
					{
						result = DeleteFiles(hDlg, pszStandardFiles, PreviousGameDirectory);
						if (result == 0)
						{
							result = DeleteFiles(hDlg, pszDataFiles, PreviousGameDirectory);
							if (result == 0)
							{
								result = DeleteFiles(hDlg, pszMovieFiles, PreviousGameDirectory);
								if (result == 0)
								{
									if (gbDeleteOldSaveGame)
									{
										result = DeleteFiles(hDlg, pszSaveGameFiles, PreviousGameDirectory);
									}
									if (result == 0)
									{
										/* Delete the previous game directory if it is empty */
										RemoveDirectory(PreviousGameDirectory);
										if (result == 0)
										{
											//SetDialogText(hDlg, IDC_ACTION, "Deleting Registry entries");
											result =  DeleteRegistryEntries();
											//SetDialogText(hDlg, IDC_ACTION, "");
										}
									}
								}
							}
						}
					}
		
					if (result == 0)
					{
						/* Success */
			            MessageBox(hDlg, APP_TITLE " removed", title, MB_OK);
		                EndDialog(hDlg, 0);
					}
					else
					{
						/* Failed to remove previous installation */
			            MessageBox(hDlg, "Could not remove " APP_TITLE, title, MB_OK | MB_ICONEXCLAMATION);
		                EndDialog(hDlg, -1);
					}
		        	break;
				}
				busy--;
        }
    }
    return 0;
}

/* **************************************************************** */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
	if (!hPrevInstance)
	{
	    hinst = hInstance;

	    /*
	     * do the setup thing, it is all one big dialog box that you show
	     * and hide things from depending on the screen
	     * we just sign on, ask where to install, and install
	     */
	    DialogBox( hInstance, "DLG_MASTER", NULL, masterDlgProc );

	}

    return 0;
} /* WinMain */


