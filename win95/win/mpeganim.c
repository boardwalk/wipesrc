void mpeganim(void)
{
	RECT prc;
	static char ReturnString[80],command[80];
	LPSTR lpszReturnString=ReturnString;	
       	dwMCIWndStyle = WS_CHILD |		// child window
            WS_VISIBLE |			// visible
            MCIWNDF_NOTIFYMODE |		// notifies of mode changes
            MCIWNDF_NOPLAYBAR;			// hides toolbar
        	                                    

#if 0
	wsprintf(command,"open \\test.mpg type mpegvideo alias wipeout parent %d",hwnd);
	mciSendString(command,
		lpszReturnString, lstrlen(lpszReturnString),NULL);
	mciSendString("play wipeout fullscreen notify",
		lpszReturnString, lstrlen(lpszReturnString),hwnd);
	MPEGplaying=TRUE;
	do
	{
		ProcessMessages();
	}while(MPEGplaying);
	mciSendString("close wipeout",
		lpszReturnString, lstrlen(lpszReturnString),NULL);
#endif
#if 1
	g_hwndMCIWnd = MCIWndCreate(hwnd,gInstance,dwMCIWndStyle,"\\test.mpg");
	(LONG)MCIWndSM(g_hwndMCIWnd, MCI_PLAY, 0,MCI_MCIAVI_PLAY_FULLSCREEN);
//	MCIWndPlay(g_hwndMCIWnd);
	MPEGplaying=TRUE;
	do
	{
		ProcessMessages();
	}while(MPEGplaying);
        MCIWndClose(g_hwndMCIWnd);
#endif
}

