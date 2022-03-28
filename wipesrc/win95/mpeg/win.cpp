#include <windows.h>
#include "mciwnd.h"

#define MCI_MCIAVI_PLAY_FULLSCREEN 0x02000000L

HINSTANCE hInstance;
HWND hwndMCIWnd;

void ShellMoviePlay(char *);

/*
 * WinMain
 *
 * Purpose:
 *  Main entry point of application.
 */

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine, int nCmdShow)
{
	hInstance = hInst;
    MSG         msg;
	ShellMoviePlay("\\wipeout.mpg");
    while (GetMessage(&msg, NULL, 0,0 ))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT FAR PASCAL VideoWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch(iMsg) {
		case MM_MCINOTIFY:
			switch(wParam) {
				case MCI_NOTIFY_ABORTED:
				case MCI_NOTIFY_SUCCESSFUL:
				case MCI_NOTIFY_SUPERSEDED:
				case MCI_NOTIFY_FAILURE:
					MCIWndClose(hwndMCIWnd);
					MCIWndDestroy(hwndMCIWnd);
					DestroyWindow(hWnd);
					break;
			}
			return 1;
		default:
			return (DefWindowProc(hWnd, iMsg, wParam, lParam));
	}
}

void ShellMoviePlay(char *pName)
{
    WNDCLASS    wc;

    wc.style          = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc    = VideoWndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = 0;
    wc.hInstance      = hInstance;
    wc.hIcon          = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "MpegVideoWindow";

    if (!RegisterClass(&wc))
        return;

	HWND m_hWnd = CreateWindow("MpegVideoWindow", "Fullscreen MPEG Demo", 
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
			NULL, NULL, hInstance, NULL);
	
    if (NULL==m_hWnd)
        return;// FALSE;

	DWORD dwMCIWndStyle = WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR | MCIWNDF_NOTIFYMODE | 
		CS_BYTEALIGNCLIENT | MCIWNDF_NOMENU | MCIWNDF_NOERRORDLG;

	hwndMCIWnd = MCIWndCreate(m_hWnd, hInstance, dwMCIWndStyle, pName);
	//SetWindowPos(NULL, x-pos, y-pos, width, height, SWP_NOZORDER);

	MCI_PLAY_PARMS playInfo;
	playInfo.dwCallback = MAKELONG(m_hWnd, 0);

	UINT id = MCIWndGetDeviceID(hwndMCIWnd);
	DWORD dwErr = mciSendCommand(id, MCI_PLAY, MCI_MCIAVI_PLAY_FULLSCREEN, (DWORD)&playInfo);
}
