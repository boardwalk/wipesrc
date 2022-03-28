/***************************************************************************
 * 
 * Assault Rigs 3D Rage Setup
 *
 ***************************************************************************/
 
#include <windows.h>
#include <shellapi.h>   // for SHFileOperation
#include <shlobj.h>     // for SHBroweForFolder
#include "dsetup.h"
#include "dinstall.h"
#include <stdio.h>
#include <regstr.h>
#include "resource.h"

/* */				  
#define DISTRIBUTION_DIR "WIPEOUT"

/* */
#define DIRECTX_SETUP_DIR "\\"

/* */
#define DIRECTX_SETUP_APP "\\directx\\dxsetup.exe"
#define DIRECTX_SETUP_APP_COMMAND_LINE ""

/* */
#define DISK_SPACE_SAFETY_MARGIN_CLUSTERS 8

/* */
#define APP_TITLE "'Wipeout for 3D Rage'"

/* */
#define REGISTRY_KEY_GROUP	  		  "SOFTWARE\\3D Rage Games"
#define REGISTRY_KEY 		  		  "SOFTWARE\\3D Rage Games\\Wipeout"
#define REGISTRY_DIRECTORY_VALUE_NAME "Game Directory"

/* */
#define UNINSTALL_KEY_NAME		"Wipeout for 3D Rage"
#define UNINSTALL_DISPLAY_NAME	"Wipeout for 3D Rage"
#define UNINSTALL_APP			"UNWIPE.EXE"

#define GAME_APP "WIPEOUT.EXE"
#define HELP_APP "WIPEOUT.HLP"

#define SAVEGAME_NAME "SAVEGAME.WSF"

/* */
#define DIALOG_DIRECTORY 0
#define DIALOG_HARDDISK  1
#define DIALOG_INSTALL   2
#define DIALOG_DIRECTX	 3
#define DIALOG_FINAL	 4

/* List of uninstall files */
static char *pszUninstallFiles[] =
{
	UNINSTALL_APP,
	NULL	
};

static char helpname[80];
static char reses[80];
static char dists[80];
static char texts[80];
static char skys[80];

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
	"WIPEOUT.EXE",
	"WIPEOUT.HLP",
	NULL	
};

/* List of movie files */
static char *pszMovieFiles[] =
{
	"WIPEOUT.MPG",
	NULL
};

// name of start menu directory
char szStartDirectory[MAX_PATH];
char szProgFilesDirectory[MAX_PATH];

/* The name of the distribution directory on the CD */
char szDistDirectory[] = DISTRIBUTION_DIR;

/* The name of the executables on the CD */
char szExeName[] = GAME_APP;
char szHelpName[80] = HELP_APP;
char szUninstallName[] = UNINSTALL_APP;

/* Registry key path */
char szRegistryKeyPath[] = REGISTRY_KEY;

/*
 * default directory to install game in
 */
static char DefaultGameDirectory[MAX_PATH];// = "C:\\3D Rage Games\\Wipeout";

/*
 * title of game installation
 */
static char title [] = "Wipeout for 3D Rage Setup";

/*
 * title of program group
 */
static char group_name[] = "3D Rage Games";
static char exe_link_name[] = "Wipeout";
static char hlp_link_name[] = "Wipeout Help";
static char uninstall_link_name[] = "Uninstall Wipeout";

/* 
 *
 */

static int gbDeleteOldSaveGame = FALSE;
static int gbCopyNewSaveGame = TRUE;

/* 
 *
 */

static DWORD gdWindowsDiskSectorsPerCluster;
static DWORD gdWindowsDiskBytesPerSector;
static DWORD gdWindowsDiskNumFreeClusters;
static DWORD gdWindowsDiskNumClusters;
static DWORD gdWindowsDiskClusterSize;
static DWORD gdWindowsDiskSpaceAvailable;

static DWORD gdDiskSectorsPerCluster;
static DWORD gdDiskBytesPerSector;
static DWORD gdDiskNumFreeClusters;
static DWORD gdDiskNumClusters;
static DWORD gdDiskClusterSize;
static DWORD gdDiskSpaceAvailable;

static BOOL gbInstallDataFiles = FALSE;
static BOOL gbInstallMovieFiles = FALSE;

static DWORD gdUninstallFileSpace;
static DWORD gdStandardFileSpace;
static DWORD gdDataFileSpace;
static DWORD gdMovieFileSpace;

static DWORD gdDiskSpaceRequired;

/*
 * prototypes
 */
BOOL FAR PASCAL masterDlgProc( HWND hdlg,DWORD message,DWORD wparam,DWORD lparam );

/*
 * globals
 */
static HANDLE hinst;
static char GameDisk[MAX_PATH];    			// where the user wants the game
static char GameDirectory[MAX_PATH];    	// where the user wants the game
static char SetupDirectory[MAX_PATH];       // where the user ran setup from
static char DistDirectory[MAX_PATH];		// where the distribution files are

static char WindowsDirectory[MAX_PATH];		// the windows directory
static char WindowsDisk[MAX_PATH];			// the disk containing the windows directory

static char PreviousGameDirectory[MAX_PATH];	// Location of any previous install

/* */

void SetDialogText(HWND hDlg, DWORD id, char *sz, ...)
{
    char ach[256];
    wvsprintf(ach, sz, (void*)(&sz+1));
    SetDlgItemText(hDlg, id, ach);
}

/* 
 * Fork an executable and wait for it
 */

BOOL Fork(char *szExeName, char *szCommandLine, char *szDirectory, BOOL bWait)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
//	char szCurrentDir[MAX_PATH];

//	GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.wShowWindow = SW_SHOW;
	if ( CreateProcess(
			szExeName,
			szCommandLine,
			NULL,
			NULL,
			0,
			0,
			NULL,
			szDirectory,
			&startupInfo,
			&processInfo) == TRUE)
	{
		if (bWait)
		{
			/* Wait for the app to complete */
			WaitForSingleObject(processInfo.hProcess, INFINITE);
			CloseHandle(processInfo.hProcess);		
		}
		else
		{
			/* Don't wait for completion */			
		}
	}
	else
	{
		/* Failed to fork app */
		return(FALSE);
	}

	return(TRUE);
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

void SetInfoText(HWND hDlg, char *sz, ...)
{
    char ach[128];
    wvsprintf(ach, sz, (void*)(&sz+1));
    SetDlgItemText(hDlg, IDC_INFO, ach);
}

void _SHFree(void *p)
{
    IMalloc *pm;
    SHGetMalloc(&pm);
    if (pm)
    {
        pm->lpVtbl->Free(pm,p);
        pm->lpVtbl->Release(pm);
    }
}


LONG CloseKeyPath(PHKEY phkeyBuffer)
{
	PHKEY phkey;
	LONG retval;
	
	/* Find last key in buffer */
	phkey = phkeyBuffer;
	while(*phkey)
	{
		phkey++;
	}

	/* Scan back through buffer closing keys */
	while (phkey > phkeyBuffer)
	{
		phkey--;
		retval = RegCloseKey(*phkey);
		if (retval != ERROR_SUCCESS)
		{
			break;
		}
		*phkey = NULL;
	}
	return (retval);
}


LONG CreateKey(HKEY hkeyParentKey, char *szKeyName, PHKEY phkeyResult)
{
	HKEY hkey;
	DWORD dwDisposition;
	LONG retval;

	/* Create the registry key */
	retval = RegCreateKeyEx(hkeyParentKey, 				// Parent key
				szKeyName, 					// Name of new key
				0, 							// Reserved
				NULL, 						// Class string
				REG_OPTION_NON_VOLATILE, 	// Options
				KEY_WRITE,					// Security access
				NULL,						// Pointer to security stuff
				&hkey,						// Our key result
				&dwDisposition);			// Disposition result
	if (retval == ERROR_SUCCESS)
	{
		*phkeyResult = hkey;
	}
	else
	{
		*phkeyResult = NULL;
	}

	return (retval);
}


LONG CreateKeyPath(HKEY hkeyRootKey, char *szKeyPath, PHKEY phkeyBuffer, long *plLength)
{
	char szKeyName[128];
	char *pKeyPath;
	char *pKey;
	PHKEY phkey;
	HKEY hkeyParent;
	LONG length;
	LONG retval;

	phkey = phkeyBuffer;
	hkeyParent = hkeyRootKey;

  	*phkey = NULL;
	length = 0;
  	
  	pKeyPath = szKeyPath;
	while (*pKeyPath)
	{
		/* Get next key name from path */
		pKey = szKeyName;
		while (*pKeyPath && (*pKeyPath != '\\'))
		{
			*pKey++ = *pKeyPath++;
		}
		*pKey = 0;
		
		/* Skip any seperator for next time */
		if (*pKeyPath)
		{
			pKeyPath++;
		}

		/* Open/create the key */
		retval = CreateKey(hkeyParent, szKeyName, phkey);
		if (retval != ERROR_SUCCESS)
		{
			/* Failed */

			/* Close any keys we managed to open */
			CloseKeyPath(phkeyBuffer);

			/* */
			length = 0;
			break;
		}

		/* Count length of path */
		length++;

		/* This key is the parent for the next one */
		hkeyParent = *phkey;
		
		/* Keep buffer null terminated */
		phkey++;
		*phkey = NULL;
	}		

	/* Return length of path */
	*plLength = length;
	
	return(retval);
}


/* 
 * Create registry entries
 */

#define MAX_REGISTRY_PATH_LENGTH 64

LONG MakeRegistryEntries(void)
{
	HKEY hkeyPath[MAX_REGISTRY_PATH_LENGTH];
	LONG retval;
	LONG pathLength;
	HKEY hkeyOurs;

	char *szMovieDir;
	char *szDataDir;

	/* Create the registry key */
	retval = CreateKeyPath(HKEY_LOCAL_MACHINE, szRegistryKeyPath, hkeyPath, &pathLength);
	if (retval == ERROR_SUCCESS)
	{
		/* Our key is the last in the path */
		hkeyOurs = hkeyPath[pathLength - 1];

		/* Set our installed game directory */
		retval = RegSetValueEx(
			hkeyOurs, 						// The key
			"Game Directory",				// Name of our value
			0,								// Reserved
			REG_SZ,							// Data type
			(CONST BYTE *)GameDirectory,	// Address of data
			strlen(GameDirectory) + 1		// Size of data
		);

		if (retval == ERROR_SUCCESS)
		{
			/* Set our movie directory */
			if (gbInstallMovieFiles)
			{
				/* Movie files have been installed */
				szMovieDir = GameDirectory;
			}
			else
			{
				/* Movie files have not been installed */
				szMovieDir = DistDirectory;
			}
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Movie Directory",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)szMovieDir,	// Address of data
				strlen(szMovieDir) + 1		// Size of data
			);
		}

		if (retval == ERROR_SUCCESS)
		{
			/* Set our data directory */
			if (gbInstallDataFiles)
			{
				/* data files have been installed */
				szDataDir = DistDirectory;
			}
			else
			{
				/* Movie files have not been installed */
				szDataDir = DistDirectory;
			}
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Data Directory",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)szDataDir,	// Address of data
				strlen(szDataDir) + 1		// Size of data
			);
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Resolution",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)reses,	// Address of data
				strlen(reses) - 2		// Size of data
			);
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Distance",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)dists,	// Address of data
				strlen(dists) - 2		// Size of data
			);
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Texture",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)texts,	// Address of data
				strlen(texts) - 3		// Size of data
			);
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Sky",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)skys,	// Address of data
				strlen(skys) - 2		// Size of data
			);
			retval = RegSetValueEx(
				hkeyOurs, 					// The key
				"Help_File",			// Name of our value
				0,							// Reserved
				REG_SZ,						// Data type
				(CONST BYTE *)szHelpName,	// Address of data
				strlen(szHelpName) - 2		// Size of data
			);
		}

		/* Close the path win or lose */
		CloseKeyPath(hkeyPath);
	}
	return (retval);
}	


int MakeUninstallRegistryEntries(void)
{
	HKEY hkeyPath[MAX_REGISTRY_PATH_LENGTH];
	LONG retval;
	LONG pathLength;
	HKEY hkeyOurs;

	char szKeyPath[MAX_PATH];

	char szDisplayName[] = UNINSTALL_DISPLAY_NAME;
	char szUninstaller[MAX_PATH];

	/* Create the uninstall registry key */
	strcpy(szKeyPath, REGSTR_PATH_UNINSTALL);
	catpath(szKeyPath, UNINSTALL_KEY_NAME);
	retval = CreateKeyPath(HKEY_LOCAL_MACHINE, szKeyPath, hkeyPath, &pathLength);
	if (retval == ERROR_SUCCESS)
	{
		/* Our key is the last in the path */
		hkeyOurs = hkeyPath[pathLength - 1];

		/* Set uninstall display name */
		retval = RegSetValueEx(
			hkeyOurs, 							// The key
			REGSTR_VAL_UNINSTALLER_DISPLAYNAME,	// Name of our value
			0,									// Reserved
			REG_SZ,								// Data type
			(CONST BYTE *)szDisplayName,		// Address of data
			strlen(szDisplayName) + 1			// Size of data
		);

		if (retval == ERROR_SUCCESS)
		{
			/* Set uninstall app */
			strcpy(szUninstaller, WindowsDirectory);
			catpath(szUninstaller, UNINSTALL_APP);

			retval = RegSetValueEx(
				hkeyOurs, 							// The key
				REGSTR_VAL_UNINSTALLER_COMMANDLINE,	// Name of our value
				0,									// Reserved
				REG_SZ,								// Data type
				(CONST BYTE *)szUninstaller,		// Address of data
				strlen(szUninstaller) + 1			// Size of data
			);
		}

		/* Close the path win or lose */
		CloseKeyPath(hkeyPath);
	}
	return (retval);
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

/*
 * Build shortcuts in the start menu
 */

// CreateLink - uses the shell's IShellLink and IPersistFile interfaces 
//   to create and store a shortcut to the specified object. 
// Returns the result of calling the member functions of the interfaces. 
// lpszPathObj - address of a buffer containing the path of the object 
// lpszPathLink - address of a buffer containing the path where the 
//   shell link is to be stored 
// lpszDesc - address of a buffer containing the description of the 
//   shell link 
 
HRESULT CreateLink(LPCSTR lpszPathObj, LPSTR lpszPathLink, LPSTR lpszDesc, LPSTR lpszWorkDir) 
{ 
    HRESULT hres; 
    IShellLink* psl; 
 
    // Get a pointer to the IShellLink interface. 
    hres = CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLink, &psl); 
    if (hres == S_OK) 
    { 
        IPersistFile* ppf; 
 
		/* Set path to target */
        psl->lpVtbl->SetPath(psl, lpszPathObj); 
		/* Set description */
        psl->lpVtbl->SetDescription(psl, lpszDesc); 
		/* Set working directory */
        psl->lpVtbl->SetWorkingDirectory(psl, lpszWorkDir);
		/* Set icon - the first icon in the target file */
        psl->lpVtbl->SetIconLocation(psl, lpszPathObj, 0);
 
		// Query IShellLink for the IPersistFile interface for saving the 
       	// shortcut in persistent storage. 
        hres = psl->lpVtbl->QueryInterface(psl, &IID_IPersistFile, &ppf); 
 
        if (hres == S_OK) 
        { 
            WORD wsz[MAX_PATH]; 
 
            // Ensure that the string is ANSI. 
            MultiByteToWideChar(CP_ACP, 0, lpszPathLink, -1, wsz, MAX_PATH); 
 
            // Save the link by calling IPersistFile::Save. 
            hres = ppf->lpVtbl->Save(ppf, wsz, TRUE); 
            ppf->lpVtbl->Release(ppf); 
        } 
        psl->lpVtbl->Release(psl); 
    } 
    return hres; 
} 
 

int MakeShortcut(char *szTargetName, char *szLinkName, char *szTargetDir)
{
	char szWindowsDir[MAX_PATH];

	char szLinkPathName[MAX_PATH];
	char szLinkGroupPathName[MAX_PATH];
	char szTargetPathName[MAX_PATH];

	DWORD dFileAttributes;

	BOOL bRetVal;
	HRESULT result;

	/* Build path name for link in start up menu */
    GetWindowsDirectory(szWindowsDir, sizeof(szWindowsDir));

	/* Build path name for link group in start up menu */
    strcpy(szLinkGroupPathName, szStartDirectory);
	catpath(szLinkGroupPathName, "\\3D Rage Games");
	
	/* Create the link group directory if it does not already exist */
	dFileAttributes = GetFileAttributes(szLinkGroupPathName);
	if (dFileAttributes == 0xffffffff)
	{
		bRetVal = CreateDirectory(
	    	szLinkGroupPathName,	// pointer to a directory path string
	    	NULL 					// pointer to a security descriptor 
		);
		if (bRetVal != TRUE)
	  	{
			/* Failed to create directory */
		   	return(-1);
		}
	}
	
	/* Build path name for link in start up menu */
    strcpy(szLinkPathName, szStartDirectory);
	catpath(szLinkPathName, "\\3D Rage Games");
	catpath(szLinkPathName, szLinkName);
	strcat(szLinkPathName, ".LNK");

	/* Build full path name for target */
    lstrcpy(szTargetPathName, szTargetDir);
	catpath(szTargetPathName, szTargetName);

	/* Create the link objects */
	result = CreateLink(szTargetPathName, szLinkPathName, szLinkName, szTargetDir);

	if (result == S_OK)
	{
		return(0);
	}
	else
	{
		return(-1);
	}
}	

int MakeShortcuts(void)
{
	char szTargetDir[MAX_PATH];
	int result;

	/* Uninstall (always in the Windows directory) */
	result = MakeShortcut(szUninstallName, uninstall_link_name, WindowsDirectory);
	if (result != 0)
	{
		return(result);
	}

	/* Directory for EXE and HLP files */
	if (gbInstallDataFiles)
	{
		/* Exe and help files have been installed */
	    lstrcpy(szTargetDir, GameDirectory);
	}
	else
	{
		/* Exe and help files are on CD */
	    lstrcpy(szTargetDir, DistDirectory);
	}

	/* The game */
	result = MakeShortcut(szExeName, exe_link_name, szTargetDir);
	if (result != 0)
	{
		return(result);
	}

	/* Help */
	result = MakeShortcut(szHelpName, hlp_link_name, szTargetDir);

	return(result);
}	


int DeleteShortcut(char *szLinkName)
{
	char szLinkPathName[MAX_PATH];
	int result;
	
	/* Build path name for link in start up menu */
	strcpy(szStartDirectory, szStartDirectory);
   // GetWindowsDirectory(szLinkPathName, sizeof(szLinkPathName));
	catpath(szLinkPathName, "\\3D Rage Games");
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
			    //GetWindowsDirectory(szLinkGroupPathName, sizeof(szLinkGroupPathName));
				strcpy(	 szLinkGroupPathName, szStartDirectory);
				catpath(szLinkGroupPathName, "\\3D Rage Games");

				/* The directory will only be removed if it is empty */
				RemoveDirectory(szLinkGroupPathName);
			}
		}
	}
	return(result);
}	

void OpenInitialDialog(HWND hDlg)
{
	/* Set our bitmap */
	SetBitmap(hDlg, IDC_STATIC, "wipeout", 160, 120);
}	

void OpenDeletingDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_DELETING), SW_SHOW);
//    ShowWindow(GetDlgItem(hDlg, IDC_ACTION), SW_SHOW);

	UpdateWindow(hDlg);
}	

void CloseDeletingDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_DELETING), SW_HIDE);
//    ShowWindow(GetDlgItem(hDlg, IDC_ACTION), SW_HIDE);
}	

void OpenDirectoryDialog(HWND hDlg)
{
	/* Show controls */
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS1), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS2), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_EDITTEXT), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_BROWSE), SW_SHOW);

	/* Disable unavailable controls */
	EnableWindow(GetDlgItem(hDlg, IDC_B), FALSE);

	/*
	 * limit the size of the input of this text field to the length of a path
	 * put the default directory to install the game into in it
	 * select the whole thing to make it easy for people to replace it
	 * set the focus to it
	 */
//	SendDlgItemMessage( hDlg, IDC_EDIT, EM_LIMITTEXT, MAX_PATH, 0L);
//	SetDlgItemText( hDlg, IDC_EDIT, DefaultGameDirectory );
	SendDlgItemMessage( hDlg, IDC_EDIT, EM_SETSEL, 0, MAKELONG(256, 256) );
	SetFocus( GetDlgItem(hDlg, IDC_EDIT) );

	UpdateWindow(hDlg);
}	

void CloseDirectoryDialog(HWND hDlg)
{
	/* Hide controls */
	ShowWindow(GetDlgItem(hDlg, IDC_EDIT), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS1), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS2), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_EDITTEXT), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_BROWSE), SW_HIDE);
}	

void OpenHardDiskDialog(HWND hDlg)
{
	/* Show controls */
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS3), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_INSTALLDATA), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLMOVIES), SW_SHOW);
	ShowWindow(GetDlgItem(hDlg, IDC_DATASIZE), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_MOVIESSIZE), SW_SHOW);

    ShowWindow(GetDlgItem(hDlg, IDC_DISKSPACE), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKSPACENUMBER), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKREQUIRED), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKREQUIREDNUMBER), SW_SHOW);

	ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_SHOW);

	/* Enable available controls */
	EnableWindow(GetDlgItem(hDlg, IDC_B), TRUE);

	UpdateWindow(hDlg);
}	

void RefreshHardDiskDialog(HWND hDlg)
{
//	SetDialogText(hDlg, IDC_DISKSPACE, "Disk space available: %dK", gdDiskSpaceAvailable);
	SetDialogText(hDlg, IDC_DISKSPACENUMBER, "%dK", gdDiskSpaceAvailable);

	gdDiskSpaceRequired = gdStandardFileSpace;
	if (gbInstallDataFiles)
	{
		if (gdDiskSpaceRequired + gdDataFileSpace < gdDiskSpaceAvailable)
		{
			gdDiskSpaceRequired += gdDataFileSpace;
		}
		else
		{
			/* This option should not be available */
			gbInstallDataFiles = FALSE;
	        SendDlgItemMessage( hDlg, IDC_INSTALLDATA, BM_SETCHECK, 0, 0L);
		}
	}

	if (gbInstallMovieFiles)
	{
		if (gdDiskSpaceRequired + gdMovieFileSpace < gdDiskSpaceAvailable)
		{
			gdDiskSpaceRequired += gdMovieFileSpace;
		}
		else
		{
			/* This option should not be available */
			gbInstallMovieFiles = FALSE;
	        SendDlgItemMessage( hDlg, IDC_INSTALLMOVIES, BM_SETCHECK, 0, 0L);
		}
	}

	if (!gbInstallDataFiles)
	{
		if (gdDiskSpaceRequired + gdDataFileSpace < gdDiskSpaceAvailable)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INSTALLDATA), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_DATASIZE), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INSTALLDATA), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_DATASIZE), FALSE);
		}
	}

	if (!gbInstallMovieFiles)
	{
		if (gdDiskSpaceRequired + gdMovieFileSpace < gdDiskSpaceAvailable)
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INSTALLMOVIES), TRUE);
			EnableWindow(GetDlgItem(hDlg, IDC_MOVIESSIZE), TRUE);
		}
		else
		{
			EnableWindow(GetDlgItem(hDlg, IDC_INSTALLMOVIES), FALSE);
			EnableWindow(GetDlgItem(hDlg, IDC_MOVIESSIZE), FALSE);
		}
	}

	SetDialogText(hDlg, IDC_DATASIZE, "(%dK)", gdDataFileSpace);
	SetDialogText(hDlg, IDC_MOVIESSIZE, "(%dK)", gdMovieFileSpace);

//	SetDialogText(hDlg, IDC_DISKREQUIRED, "Disk space required: %dK", gdDiskSpaceRequired);
	SetDialogText(hDlg, IDC_DISKREQUIREDNUMBER, "%dK", gdDiskSpaceRequired);

	UpdateWindow(hDlg);
};

void CloseHardDiskDialog(HWND hDlg)
{
	/* Hide controls */
	ShowWindow(GetDlgItem(hDlg, IDC_DIRECTIONS3), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_INSTALLDATA), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLMOVIES), SW_HIDE);
	ShowWindow(GetDlgItem(hDlg, IDC_DATASIZE), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_MOVIESSIZE), SW_HIDE);

    ShowWindow(GetDlgItem(hDlg, IDC_DISKSPACE), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKSPACENUMBER), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKREQUIRED), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_DISKREQUIREDNUMBER), SW_HIDE);

	ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_HIDE);
}	

void OpenInstallDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALL), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_SHOW);

	/* Enable available controls */
    EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_B), TRUE);

	UpdateWindow(hDlg);
}

void CloseInstallDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALL), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_HIDE);
}

void OpenInstallingDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLING), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLACTION), SW_SHOW);
//    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_SHOW);

	/* Disable unavailable controls */
    EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);
	EnableWindow(GetDlgItem(hDlg, IDC_B), FALSE);

	UpdateWindow(hDlg);
}

void CloseInstallingDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLING), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLACTION), SW_HIDE);
//    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_HIDE);
}


void OpenDirectXDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_DIRECTXDIRECTIONS), SW_SHOW);
    //ShowWindow(GetDlgItem(hDlg, IDC_DIRECTXDIRECTIONS2), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLDIRECTX), SW_SHOW);
    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_SHOW);

	/* Disable unavailable controls */
	EnableWindow(GetDlgItem(hDlg, IDC_B), FALSE);

	/* Enable controls */
    EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), TRUE);

	UpdateWindow(hDlg);
}

void CloseDirectXDialog(HWND hDlg)
{
    ShowWindow(GetDlgItem(hDlg, IDC_DIRECTXDIRECTIONS), SW_HIDE);
	//ShowWindow(GetDlgItem(hDlg, IDC_DIRECTXDIRECTIONS2), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_INSTALLDIRECTX), SW_HIDE);
    ShowWindow(GetDlgItem(hDlg, IDC_INFO), SW_HIDE);
}

void OpenFinalDialog(HWND hDlg)
{
	/* Set our bitmap */
	SetBitmap(hDlg, IDC_STATIC, "rage", 160, 120);

	/* Show controls */
    ShowWindow( GetDlgItem(hDlg, IDC_SUCCESS), SW_SHOW );
    SetWindowText( GetDlgItem(hDlg, IDOK), "&Finish" );

	/* Disable unavailable controls */
	EnableWindow(GetDlgItem(hDlg, IDC_B), FALSE);
    EnableWindow(GetDlgItem(hDlg, IDCANCEL), FALSE);

	/* Enable available controls */
    EnableWindow(GetDlgItem(hDlg,IDOK), TRUE);

	UpdateWindow(hDlg);
}	

void CloseFinalDialog(HWND hDlg)
{
}	

int CopyFiles(HWND hDlg, char **pszList, char *szSrcDir, char *szDstDir)
{
	char **pszNext;
	char *szName;

    char src[MAX_PATH];
    char dst[MAX_PATH];
    SHFILEOPSTRUCT fileop;

    int result;

	pszNext = pszList;
	result = 0;
	while (*pszNext && (result == 0))
	{
		szName = *pszNext++;

        lstrcpy(src, szSrcDir);
		catpath(src, szName);

        lstrcpy(dst, szDstDir);
        catpath(dst, szName);

		SetDialogText(hDlg, IDC_INSTALLACTION, "Copying %s", szName);

        fileop.hwnd     = hDlg;
        fileop.wFunc    = FO_COPY;
        fileop.pFrom    = src;
        fileop.pTo      = dst;
        fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

        while (result = SHFileOperation(&fileop))
        {
            char errorTemplate[] = "Setup Failure: %s could not be copied.";
            char errorText[MAX_PATH+sizeof(errorTemplate)];

            wsprintf(errorText, errorTemplate, szName);
            result = MessageBox(hDlg, errorText, title, MB_RETRYCANCEL);

            if(result == IDCANCEL)
            {
                result = -1;
                break;
            }
        }

        if( result == 0 )
        {
            SetFileAttributes( dst, FILE_ATTRIBUTE_NORMAL );
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
			/* Delete it */
			SetDialogText(hDlg, IDC_ACTION, "Deleting %s", szName);

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

int GetFileSpaceRequired(char **pszList, DWORD clusterSize, DWORD *pdSpaceRequired)
{
	char **pszNext;
	char *szName;

    char src[MAX_PATH];

	HANDLE hFile;

	DWORD fileLength;
	DWORD fileClusters;
	DWORD fileSpace;

	DWORD spaceRequired;

	int result;
	
	/* */
	spaceRequired = 0;

	pszNext = pszList;
	result = 0;
	while (*pszNext && (result == 0))
	{
		szName = *pszNext++;

        lstrcpy(src, DistDirectory);
		catpath(src, szName);

		/* Get the length of the file */
		hFile = CreateFile( (LPCTSTR)src,				// name
							GENERIC_READ,				// access
						    0,							// no sharing
				    		NULL,						// security attributes
				    		OPEN_EXISTING,				// creation mode
						    FILE_ATTRIBUTE_NORMAL,		// attributes
						    NULL						// template file
		);

		if (hFile == INVALID_HANDLE_VALUE)
		{
		 	result = -1;
			break;
		}

		fileLength = GetFileSize(hFile, NULL);

		CloseHandle(hFile);	

		if (fileLength == 0xffffffff)
		{
			result = -1;
			break;
		}

		/* Calculate  disk space requirements */
		fileClusters = (fileLength + clusterSize - 1) / clusterSize;
		fileSpace = fileClusters * clusterSize;

		/* Sum space required for all files */
		spaceRequired += fileSpace;
	}

	/* Return space required */
	*pdSpaceRequired = (spaceRequired + 1023) / 1024;
	return (result);
}	

int GetDiskSpace(char *szRootPath)
{
	int retval;

	/* Get windows disk space info */
	retval = GetDiskFreeSpace(
    	(LPCTSTR)WindowsDisk,						// root path 
    	(LPDWORD)&gdWindowsDiskSectorsPerCluster,	// sectors per cluster 
	    (LPDWORD)&gdWindowsDiskBytesPerSector,		// bytes per sector 
    	(LPDWORD)&gdWindowsDiskNumFreeClusters,		// number of free clusters  
	    (LPDWORD)&gdWindowsDiskNumClusters		 	// total number of clusters  
   	);
	if (retval != TRUE)
	{
		return(-1);
	}

	/* Calc free space on windows disk */
	gdWindowsDiskClusterSize = gdWindowsDiskBytesPerSector * gdWindowsDiskSectorsPerCluster;
	gdWindowsDiskSpaceAvailable = (gdWindowsDiskClusterSize * gdWindowsDiskNumFreeClusters + 1023) / 1024;

	/* Get disk space info */
	retval = GetDiskFreeSpace(
    	(LPCTSTR)szRootPath,				// root path 
    	(LPDWORD)&gdDiskSectorsPerCluster,	// sectors per cluster 
	    (LPDWORD)&gdDiskBytesPerSector,		// bytes per sector 
    	(LPDWORD)&gdDiskNumFreeClusters,	// number of free clusters  
	    (LPDWORD)&gdDiskNumClusters		 	// total number of clusters  
   	);
	if (retval != TRUE)
	{
		return(-1);
	}

	/* Calc free space on disk */
	gdDiskClusterSize = gdDiskBytesPerSector * gdDiskSectorsPerCluster;
	gdDiskSpaceAvailable = (gdDiskClusterSize * gdDiskNumFreeClusters + 1023) / 1024;

	return(0);
}	

int GetDiskRequirements(char *szRootPath)
{
	int retval;
	DWORD dwTemp;

	/* Get requirements for uninstall files */
	retval = GetFileSpaceRequired((char **)pszUninstallFiles, gdWindowsDiskClusterSize, &gdUninstallFileSpace);
	if (retval != 0)
	{
		return(-1);
	}
	/* Add safety margin */
	gdUninstallFileSpace += (DISK_SPACE_SAFETY_MARGIN_CLUSTERS * gdWindowsDiskClusterSize + 1023) / 1024;

	/* Get requirements for standard and savegame files */
	retval = GetFileSpaceRequired(pszStandardFiles, gdDiskClusterSize, &gdStandardFileSpace);
	if (retval != 0)
	{
		return(-1);
	}
	retval = GetFileSpaceRequired(pszSaveGameFiles, gdDiskClusterSize, &dwTemp);
	if (retval != 0)
	{
		return(-1);
	}
	gdStandardFileSpace += dwTemp;

	/* Add safety margin */
	gdStandardFileSpace += (DISK_SPACE_SAFETY_MARGIN_CLUSTERS * gdDiskClusterSize + 1023) / 1024;

	/* Get requirements for data files */
	retval = GetFileSpaceRequired(pszDataFiles, gdDiskClusterSize, &gdDataFileSpace);
	if (retval != 0)
	{
		return(-1);
	}

	/* Get requirements for movie files */
	retval = GetFileSpaceRequired(pszMovieFiles, gdDiskClusterSize, &gdMovieFileSpace);
	if (retval != 0)
	{
		return(-1);
	}

	/* If the Windows disk is the same as the game disk */
	/* we count the uninstall files as standard files   */
	/* to simplify requirements checking 				*/
	if (strcmp(GameDisk, WindowsDisk) == 0)
	{
		gdStandardFileSpace += gdUninstallFileSpace;
		gdUninstallFileSpace = 0;
	}

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

int SaveGameExists(void)
{
	char szSaveGamePathName[MAX_PATH];
	DWORD dwAttributes;
	int result;

	/* Default to no existing save game */
	result = FALSE;

	/* Look for a save game file in the proposed game directory */
	strcpy(szSaveGamePathName, GameDirectory);
	catpath(szSaveGamePathName, SAVEGAME_NAME);

	dwAttributes = GetFileAttributes(szSaveGamePathName);
	if (dwAttributes != 0xffffffff)
	{
		/* Save game file exists */
		result = TRUE;
	}

	return(result);
}	


/*
 * dlg proc for wizard dialog box, the setup is controlled from here.
 */
BOOL FAR PASCAL masterDlgProc(HWND hDlg,DWORD dwMessage,DWORD wParam,DWORD lParam)
{
    int         result;
//    static int  system_restart;
    static int  current_dialog;
    static int  busy;

	RECT rc;
	int w,h;

    char        src[MAX_PATH];
    char        dst[MAX_PATH];
    SHFILEOPSTRUCT fileop;

	LONG		retval;

    switch(dwMessage)
    {
    case WM_INITDIALOG:
        busy = 0;
        current_dialog = DIALOG_DIRECTORY;
      	SetWindowText(hDlg, title);

		/* No help yet */
	    EnableWindow( GetDlgItem(hDlg, IDC_H), FALSE );
	
		busy++;

        /*
         * limit the size of the input of this text field to the length of a path
         * put the default directory to install the game into it
         */
        SendDlgItemMessage( hDlg, IDC_EDIT, EM_LIMITTEXT, MAX_PATH, 0L);
        SetDlgItemText( hDlg, IDC_EDIT, DefaultGameDirectory );

		/* Setup the dialog */
		OpenInitialDialog(hDlg);

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
		if (result == ERROR_SUCCESS)
		{
            result = MessageBox(
	            hDlg, 
	            "Setup has detected a previous installation of " APP_TITLE ".\nDo you want to replace this?", 
	            title, 
	            MB_YESNO | MB_DEFBUTTON2
            );
			if (result == IDNO)
			{
				/* Abort installation */				
                EndDialog(hDlg, 0);
				busy--;
				break;
			}
			else
			{
	            result = MessageBox(hDlg, "Do you want to keep your saved game files?", title, MB_YESNOCANCEL | MB_DEFBUTTON1);
				if (result == IDCANCEL)
				{
					/* Abort installation */				
    	            EndDialog(hDlg, 0);
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

					/* Delete existing installation */
					OpenDeletingDialog(hDlg);

					SetDialogText(hDlg, IDC_ACTION, "Deleting StartMenu shortcuts");
					result = DeleteShortcuts();
					SetDialogText(hDlg, IDC_ACTION, "");
					if (result == 0)
					{
						result = DeleteFiles(hDlg, pszUninstallFiles, WindowsDirectory);
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
												SetDialogText(hDlg, IDC_ACTION, "Deleting Registry entries");
												result =  DeleteRegistryEntries();
												SetDialogText(hDlg, IDC_ACTION, "");
											}
										}
									}
								}
							}
						}
					}

					/* Failed to remove previous installation */
					if (result != 0)
					{
			            MessageBox(
		            		hDlg, 
		            		"Setup could not remove previous installation of " APP_TITLE, 
		            		title, 
		            		MB_OK | MB_ICONEXCLAMATION
	            		);
		                EndDialog(hDlg, -1);
						busy--;
						break;
					}

					CloseDeletingDialog(hDlg);
	
					/* Set the default install directory to the same place as */
					/* the previous installation 							  */
			        SetDlgItemText( hDlg, IDC_EDIT, PreviousGameDirectory);
				}
			}
		}

		/* */
		OpenDirectoryDialog(hDlg);

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
    	case IDC_B:
			if (busy > 0)
			{
				break;
			}

			switch (current_dialog)
			{
				case DIALOG_DIRECTORY:
					break;

				case DIALOG_HARDDISK:
					CloseHardDiskDialog(hDlg);
                    current_dialog = DIALOG_DIRECTORY;
					OpenDirectoryDialog(hDlg);
					break;

				case DIALOG_INSTALL:
					CloseInstallDialog(hDlg);
                    current_dialog = DIALOG_HARDDISK;
					OpenHardDiskDialog(hDlg);
					break;

				case DIALOG_DIRECTX:
					break;

				case DIALOG_FINAL:
					break;
			}
    		break;
    
    
    
        case IDOK:
            if( busy > 0 )
            {
                /*
                 * busy bit keeps us from taking input while we are off doing
                 * things that can create other dialog boxes and end up causing
                 * us to be reentered.
                 */
                break;
            }

			switch (current_dialog)
			{
				case DIALOG_DIRECTORY:
				{
	                busy++;

	                /* Get the directory the user typed */
	                GetWindowText( GetDlgItem( hDlg,IDC_EDIT ), GameDirectory, sizeof(GameDirectory));

					/* Check that path includes a drive */
					if ( ( strlen(GameDirectory) < 2 ) || 
					     ( GameDirectory[1] != ':' ) || 
					     ( toupper(GameDirectory[0]) < 'A') ||
					     ( toupper(GameDirectory[0]) > 'Z') )
					{
						/* Report invalid drive */
	                    MessageBox(hDlg, 
	                               "You must specify a valid drive onto which to install " APP_TITLE, 
	                               title, 
	                               MB_OK );
						/* */
						busy--;
						break;
					}

					/* Get disk intended for install */
					GameDisk[0] = toupper(GameDirectory[0]);
					GameDisk[1] = ':';
					GameDisk[2] = '\\';
					GameDisk[3] = 0;

					/* Check the installation disk by trying to find out how much space there is */
	                SetInfoText(hDlg, "Checking disk space.");
					result = GetDiskSpace(GameDisk);
	                SetInfoText(hDlg, "");
					if (result != 0)
					{
						/* Report invalid drive */
	                    MessageBox(hDlg, 
	                               "You must specify a valid drive onto which to install " APP_TITLE, 
	                               title, 
	                               MB_OK );
						/* */
						busy--;
						break;
					}

					/* Get disk space requirements */
	                SetInfoText(hDlg, "Checking disk space.");
					result = GetDiskRequirements(GameDisk);
	                SetInfoText(hDlg, "");

					/* Ensure there is enough space for installation */
					/* of uninstall files 							 */
					/* (If Windows disk is different to requested    */
					/* game disk)									 */
					if (gdUninstallFileSpace > gdWindowsDiskSpaceAvailable)
					{
						char szMessage[128];

						/* Report insufficient space */
						wsprintf(szMessage, "There is insufficient space in the Windows directory on drive %c: to install " APP_TITLE, WindowsDisk[0]);
	                    MessageBox(hDlg, szMessage, title, MB_OK);
						/* */
						busy--;
						break;
					}

					/* Next dialog */
					CloseDirectoryDialog(hDlg);
                    current_dialog = DIALOG_HARDDISK;
					OpenHardDiskDialog(hDlg);

					/* */
					RefreshHardDiskDialog(hDlg);

	                busy--;
					break;
				}

				case DIALOG_HARDDISK:

	                busy++;

					/* Ensure there is enough space for standard installation */
					if (gdStandardFileSpace > gdDiskSpaceAvailable)
					{
						char szMessage[128];

						/* Report insufficient space */
						wsprintf(szMessage, "There is insufficient space on drive %c: to install " APP_TITLE , GameDisk[0]);
	                    MessageBox(hDlg, szMessage, title, MB_OK);
						/* */
						busy--;
						break;
					}

					/* Check for existing save game file */
					if (SaveGameExists())
					{
	                    result = MessageBox(hDlg, "Setup has detected an existing save game file.\nDo you want keep this?", title, MB_YESNOCANCEL | MB_DEFBUTTON1);
						if (result == IDCANCEL)
						{
							busy--;
							break;
						}
						else
						{
							if (result == IDYES)
							{
								gbCopyNewSaveGame = FALSE;
							}
							else
							{
								gbCopyNewSaveGame = TRUE;
							}
						}
					}

					/* */
					CloseHardDiskDialog(hDlg);
                    current_dialog = DIALOG_INSTALL;
					OpenInstallDialog(hDlg);

	                busy--;
					break;

				case DIALOG_INSTALL:
				{
	                busy++;
						
					CloseInstallDialog(hDlg);
					OpenInstallingDialog(hDlg);

	                SetCursor(LoadCursor(NULL, IDC_WAIT));

	                /*
	                 * verify that the typed in directory is valid
	                 * by having the SHELL copy WIN.INI to this directory
	                 * it will also create the directory for us.
	                 */
					SetDialogText(hDlg, IDC_INSTALLACTION, "Creating directory");

	                GetWindowsDirectory(src, sizeof(src));
	                catpath(src,"WIN.INI");

	                lstrcpy(dst,GameDirectory);
	                catpath(dst,"WIPEOUT.TMP");

	                fileop.hwnd     = hDlg;
	                fileop.wFunc    = FO_COPY;
	                fileop.pFrom    = src;
	                fileop.pTo      = dst;
	                fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

	                if (SHFileOperation(&fileop) != 0)
	                {
	                    /* failed, the shell gave the user a error. */
						SetDialogText(hDlg, IDC_INSTALLACTION, "");

						/* Back up */
						CloseInstallingDialog(hDlg);
						OpenInstallDialog(hDlg);
	                    busy--;
	                    break;
	                }

	                /*
	                 * the directory is valid now delete the bogus file
	                 */
	                fileop.hwnd     = hDlg;
	                fileop.wFunc    = FO_DELETE;
	                fileop.pFrom    = dst;
	                fileop.pTo      = NULL;
	                fileop.fFlags   = FOF_SILENT | FOF_NOCONFIRMATION;

	                SHFileOperation(&fileop);
					SetDialogText(hDlg, IDC_INSTALLACTION, "");

					/* Create our registry entries */
					SetDialogText(hDlg, IDC_INSTALLACTION, "Creating Registry entries");
					retval = MakeRegistryEntries();
					if (retval == ERROR_SUCCESS)
					{
						retval = MakeUninstallRegistryEntries();
					}
					if (retval != ERROR_SUCCESS)
					{
						/* Report failure */
	                    MessageBox(hDlg, 
	                               "Setup Failure: could not create registry entries", 
	                               title, 
	                               MB_OK );
						/* Fail setup */
						result = -1;
					}
					SetDialogText(hDlg, IDC_INSTALLACTION, "");

					/* Provided all is well */
					if (result >= 0)
					{
		                /* Copy any files */
						SetDialogText(hDlg, IDC_INSTALLACTION, "Copying files.");
						/* Uninstall files */
						result = CopyFiles(hDlg, pszUninstallFiles, DistDirectory, WindowsDirectory);
						/* Standard files */
						if (result == 0)
						{
							result = CopyFiles(hDlg, pszStandardFiles, DistDirectory, GameDirectory);
						}
						/* Save game files */
						if (gbCopyNewSaveGame)
						{
							if (result == 0)
							{
								result = CopyFiles(hDlg, pszSaveGameFiles, DistDirectory, GameDirectory);
							}
						}
						/* Game program and data files */
						if (result == 0)
						{
							if (gbInstallDataFiles)
							{
								result = CopyFiles(hDlg, pszDataFiles, DistDirectory, GameDirectory);
							}
						}
						/* Movie files */
						if (result == 0)
						{
							if (gbInstallMovieFiles)
							{
								result = CopyFiles(hDlg, (char **)&pszMovieFiles, DistDirectory, GameDirectory);
							}
						}
						SetDialogText(hDlg, IDC_INSTALLACTION, "");
					}

					/* Provided all is well */
					if (result >= 0)
					{
						/* Create our shortcuts */
						SetDialogText(hDlg, IDC_INSTALLACTION, "Creating StartMenu shortcuts");
		                result = MakeShortcuts();
						SetDialogText(hDlg, IDC_INSTALLACTION, "");
					}

	                if( result >= 0 )
	                {
						CloseInstallingDialog(hDlg);
						current_dialog = DIALOG_DIRECTX;
						OpenDirectXDialog(hDlg);

                        busy--;
                        break;
	                }
					else
					{
		                EnableWindow(GetDlgItem(hDlg,IDOK), TRUE);
		                EnableWindow(GetDlgItem(hDlg,IDCANCEL), TRUE);

		                busy--;
	                    EndDialog( hDlg, result );
					}
					break;

				case DIALOG_DIRECTX:
		            if( busy > 0 )
		            {
		                break;
		            }
	                busy++;

					CloseDirectXDialog(hDlg);
                    current_dialog = DIALOG_FINAL;
					OpenFinalDialog(hDlg);

                    busy--;
					break;
						
				case DIALOG_FINAL:
	                /*
	                 * restart windows, kill apps that aren't responding, reboot
	                 */
//	                if( system_restart )
//	                {
//	                    ExitWindowsEx( EWX_REBOOT, 0 );
//	                }
//	                else
//	                {
	                    EndDialog( hDlg, 0 );
//	                }
					break;
				}

				/* This should not be happening */
				default:
					break;
			}
            break;

        case IDCANCEL:
            if( !busy )
            {
                /*
                 * only allow cancel if we aren't doing anything else
                 */
                EndDialog( hDlg, -1 );
            }
            break;

        case IDC_BROWSE:
            if( current_dialog == DIALOG_DIRECTORY )
            {
                BROWSEINFO bi;
                LPITEMIDLIST pidl;
                char ach[MAX_PATH];

                bi.hwndOwner      = hDlg;
                bi.pidlRoot       = NULL;
                bi.pszDisplayName = ach;
                bi.lpszTitle      = NULL;
                bi.ulFlags        = BIF_RETURNONLYFSDIRS;
                bi.lpfn           = NULL;
                bi.lParam         = 0;
                bi.iImage         = 0;

                pidl = SHBrowseForFolder(&bi);

                if (pidl)
                {
                    SHGetPathFromIDList(pidl, ach);
                    SetDlgItemText(hDlg, IDC_EDIT, ach);
                    _SHFree(pidl);
                }
            }
            break;

		case IDC_INSTALLDATA:
            if( busy > 0 )
            {
                break;
            }
			if (current_dialog == DIALOG_HARDDISK)
			{
				gbInstallDataFiles = !gbInstallDataFiles;
			}
			RefreshHardDiskDialog(hDlg);
			break;
	
		case IDC_INSTALLMOVIES:
            if( busy > 0 )
            {
                break;
            }
			if (current_dialog == DIALOG_HARDDISK)
			{
				gbInstallMovieFiles = !gbInstallMovieFiles;
			}
			RefreshHardDiskDialog(hDlg);
			break;

		case IDC_INSTALLDIRECTX:
            if( busy > 0 )
            {
                break;
            }
			if (current_dialog == DIALOG_DIRECTX)
			{
				char szCurrentDir[MAX_PATH];

				busy++;

				/* Fork the DirectX setup app but don't wait for it */
				GetCurrentDirectory(sizeof(szCurrentDir), szCurrentDir);
				Fork(DIRECTX_SETUP_APP, DIRECTX_SETUP_APP_COMMAND_LINE, szCurrentDir, FALSE);
	
				///* Move on */
				//CloseDirectXDialog(hDlg);
                //current_dialog = DIALOG_FINAL;
				//OpenFinalDialog(hDlg);

				busy--;

				/* Terminate because the DirectX setup may need to reboot and we */
				/* shouldn't hang around waiting for it                          */
                EndDialog( hDlg, 0 );

			}
			break;

        }
    }	   
    return 0;
}

//pointer to imalloc interface
LPMALLOC g_pMalloc;

void getStartMenuName()
{
	LPITEMIDLIST pidlPrograms;

	// Get the shell's allocator. 
    if (!SUCCEEDED(SHGetMalloc(&g_pMalloc))) 
        return;



	//get the "Programs" directory name
	if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, 
            CSIDL_PROGRAMS, &pidlPrograms))) 
	{		
		SHGetPathFromIDList(pidlPrograms, szStartDirectory);
	}

	
}

static void GetWindowsDrive(void)
{
	/* Get the windows directory */
    GetWindowsDirectory(WindowsDirectory, sizeof(WindowsDirectory));

	/* Get the windows disk */
	WindowsDisk[0] = toupper(WindowsDirectory[0]);
	WindowsDisk[1] = ':';
	WindowsDisk[2] = '\\';
}


static void GetDefaultGameDir(void)
{
	sprintf(DefaultGameDirectory,"%s3D Rage Games\\WipEout",   WindowsDisk);
}
 
void trim()
{
	char *c;

	c = &helpname[0];
	while (*c != '\n' && *c != '\0' && *c != 0x0a)
		c++;
	*c = '\0';
	c = &reses[0];
	while (*c != '\n' && *c != '\0' && *c != 0x0a)
		*c++;
	*c = '\0';

	c = &dists[0];
	while (*c != '\n' && *c != '\0' && *c != 0x0a)
		* c++;
	*c = '\0';

	c = &texts[0];
	while (*c != '\n' && *c != '\0' && *c != 0x0a)
		*c++;
	*c = '\0';

	c = &skys[0];
	while (*c != '\n' && *c != '\0' && *c != 0x0a)
		*c++;
	*c = '\0';

}

/* **************************************************************** */
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int nCmdShow)
{
    TCHAR * p;
    TCHAR * x;

	HRESULT result;

	if (!hPrevInstance)
	{
	    hinst = hInstance;

		GetWindowsDrive();
		getStartMenuName();
		GetDefaultGameDir();

		
	    /*
	     * get our fullpath name and strip the file name
	     */
	    GetModuleFileName(hInstance, SetupDirectory, sizeof(SetupDirectory));

	    for (x=p=SetupDirectory; *p; p=AnsiNext(p))
	    {
	        if (*p == '\\' || *p == '/')
	            x = p;
	    }
	    *x = 0;	 
	//	strcpy(SetupDirectory,"d:\\wipesrc\\win57\\install\\inst");

		/* Set distribution files directory */
		lstrcpy(DistDirectory, SetupDirectory);
		catpath(DistDirectory, szDistDirectory);



		{
			// New bit for help file install
			FILE *fp;
			char IniFile[200];
			short len;

			//get help file name from command line
			strcpy(szHelpName, (char *)szCmdLine);
			len=strlen(szHelpName);

			sprintf(IniFile,"%s\\wipeout.ini",DistDirectory);
			fp=fopen(IniFile,"r");

			fgets(helpname,80,fp);
			if (len < 1)
			{
				if(helpname[strlen(helpname)-1]=='\n')
					helpname[strlen(helpname)-1]=0;
				strcpy(szHelpName, helpname);
			}
			pszDataFiles[1]= szHelpName;
			fgets(reses,80,fp);
			fgets(dists,80,fp);
			fgets(texts,80,fp);
			fgets(skys,80,fp);
			fclose(fp);
			trim();
		/*	len=strlen(helpname);
			if(helpname[len-1]=='\n')
					helpname[len-1]=0;
			len=strlen(reses);
			if(reses[len-1]=='\n')
					reses[len-1]=0;
			len=strlen(dists);
			if(dists[len-1]=='\n')
					dists[len-1]=0;
			len=strlen(texts);
			if(texts[len-1]=='\n')
					texts[len-1]=0;
			len=strlen(skys);
			if(skys[len-1]=='\n')
					skys[len-1]=0;
			pszDataFiles[1]=helpname;
			strcpy(szHelpName,helpname); */

			
		}
			
		

		/* Initialise COM */
		result = CoInitialize(NULL);

	    /*
	     * do the setup thing, it is all one big dialog box that you show
	     * and hide things from depending on the screen
	     * we just sign on, ask where to install, and install
	     */
	    DialogBox( hInstance, "DLG_MASTER", NULL, masterDlgProc );

		/* Terminate COM */
		CoUninitialize();
	}

    return 0;
} /* WinMain */


