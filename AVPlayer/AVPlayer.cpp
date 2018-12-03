// AVPlayer.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "AVPlayer.h"
#include "AVPlayerWnd.h"
#include <Shellapi.h>
#include <DbgHelp.h>
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Dbghelp.lib")

wstring GetSkinPath(LPCTSTR lpszFolderName = nullptr)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
#ifdef _DEBUG
	PathAppend(szPath, L"..\\skin");
#else	
	PathAppend(szPath, L"skin");
#endif // DEBUG
	if (lpszFolderName)
	{
		PathAddBackslash(szPath);
		PathAppend(szPath, lpszFolderName);
	}
	PathAddBackslash(szPath);
	return szPath;
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)
{
	// 创建Dump文件  
	//  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	// Dump信息  
	//  
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	// 写入Dump文件内容  
	//  
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	// 这里弹出一个错误对话框并退出程序  
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	PathAppend(szPath, L"minidump.dmp");
	CreateDumpFile(szPath, pException);
	//FatalAppExit(-1, _T("*** Unhandled Exception! ***"));
	return EXCEPTION_EXECUTE_HANDLER;
}

int ParseCmdLine(const wchar_t * lpCmdLine, std::map<std::wstring, std::wstring>& pMapCmdLine)
{
	int nArgs = 0;
	LPWSTR * szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);
	for (int i = 0; i < nArgs; i++)
	{
		if (wcsncmp(L"//", szArglist[i], 1) != 0)
			continue;

		if (i + 1 < nArgs) //结束  
		{
			if (wcsncmp(L"//", szArglist[i + 1], 1) != 0)
			{
				pMapCmdLine.insert(std::make_pair(szArglist[i], szArglist[i + 1]));
				i++;
				continue;
			}
		}
		pMapCmdLine.insert(std::make_pair(szArglist[i], L"1"));
	}
	LocalFree(szArglist);
	return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。
	//设置Dump
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(GetSkinPath().c_str());

	std::map<std::wstring, std::wstring>	mapCmd;
	ParseCmdLine(GetCommandLineW(), mapCmd);

	auto itor = mapCmd.find(L"/is_child_wnd");
	if (itor != mapCmd.end() && itor->second.compare(L"true") == 0)
	{
		auto wndName = mapCmd.find(L"/wnd_name");
		if (wndName != mapCmd.end())
		{
			CVideoWnd* pVideoWnd = new CVideoWnd();
			pVideoWnd->SetCmdLine(std::move(mapCmd));
			pVideoWnd->Create(nullptr, wndName->second.c_str(), WS_POPUP, UI_WNDSTYLE_EX_DIALOG);
			pVideoWnd->ShowModal();
		}
	}
	else
	{
		CAVPlayerWnd player;
		player.SetCmdLine(mapCmd);
		player.Create(NULL, L"AVPlayer", UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
		player.CenterWindow();
		player.ShowModal();
	}
	
	//CPaintManagerUI::MessageLoop();
    return 0;
}
