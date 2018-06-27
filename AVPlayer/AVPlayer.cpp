// AVPlayer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "AVPlayer.h"
#include "AVPlayerWnd.h"

wstring GetSkinPath(LPCTSTR lpszFolderName = nullptr)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
#ifdef _DEBUG
	PathAppend(szPath, L"..\\skin");
#else	
	PathAppend(szPath, L"..\\skin");
#endif // DEBUG
	if (lpszFolderName)
	{
		PathAddBackslash(szPath);
		PathAppend(szPath, lpszFolderName);
	}
	PathAddBackslash(szPath);
	return szPath;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(GetSkinPath().c_str());
   
	CAVPlayerWnd player;
	player.Create(NULL, L"AVPlayer", UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
	player.CenterWindow();
	player.ShowWindow();

	CPaintManagerUI::MessageLoop();
    return 0;
}
