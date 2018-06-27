// AVPlayerTest.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "AVPlayerTest.h"
#include <shellapi.h>
#include <Shlwapi.h>

#include <Dbghelp.h>
#include <map>
#include <string>
using namespace std;

#pragma comment(lib, "Shlwapi.lib")
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Dbghelp.lib")

#define MAX_LOADSTRING 100

enum VIDEO_TYPE
{
	NORMAL_TYPE,
	PANORAMIC_TYPE,
};

typedef struct _PLAYER_OPTS
{
	void*		user_data = nullptr;
	string		strPath;
	HWND		hWnd = nullptr;
	bool		bEnableVideo = true;
	bool		bEnableAudio = true;
	VIDEO_TYPE	video_type = NORMAL_TYPE;
	HINSTANCE	hInstance;
} PLAYER_OPTS;

typedef void(*funcInit)();
typedef void(*funcUnInit)();
typedef HANDLE(*funcCreatePlayer)();
typedef void(*funcDestoryPlayer)(HANDLE handle);
typedef bool(*funcSetOptions)(HANDLE handle, PLAYER_OPTS &opts);
typedef bool(*funcPlay)(HANDLE handle, PLAYER_OPTS &opts);
typedef void(*funcStop)(HANDLE handle);
typedef bool(*funcIsPlaying)(HANDLE handle);
typedef void(*funcSetWindowSize)(HANDLE handle, int nWidth, int nHeight);

typedef void(*funcSetPanoScale)(HANDLE handle, float factor);
typedef void(*funcSetPanoRotate)(HANDLE handle, float x, float y);
typedef void(*funcSetPanoScroll)(HANDLE handle, float latitude, float longitude);

typedef struct _MPLAYER
{
	funcInit		m_funcInit;
	funcUnInit		m_funcUnInit;
	funcCreatePlayer	m_funcCreatePlayer = nullptr;
	funcDestoryPlayer	m_funcDesotryPlayer = nullptr;
	funcSetOptions		m_funcSetOptions = nullptr;
	funcSetPanoScale    m_funcSetPanoScale = nullptr;
	funcSetPanoRotate	m_funcSetPanoRotate = nullptr;
	funcSetPanoScroll	m_funcSetPanoScroll = nullptr;
	funcPlay			m_funcPlay = nullptr;
	funcStop			m_funcStop = nullptr;
	funcIsPlaying		m_funcIsPlaying = nullptr;
	funcSetWindowSize	m_funcSetWindowSize = nullptr;
	HANDLE			m_hPlayer = nullptr;
}MPlayer;

std::wstring GetAppPath()
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAddBackslash(szPath);
	return szPath;
}

HMODULE hPlayerModule = NULL;

// ȫ�ֱ���: 
HINSTANCE hInst;                                // ��ǰʵ��
WCHAR szTitle[MAX_LOADSTRING];                  // �������ı�
WCHAR szWindowClass[MAX_LOADSTRING];            // ����������

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: �ڴ˷��ô��롣

    // ��ʼ��ȫ���ַ���
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_AVPLAYERTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ִ��Ӧ�ó����ʼ��: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AVPLAYERTEST));

    MSG msg;

    // ����Ϣѭ��: 
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//
//  ����: MyRegisterClass()
//
//  Ŀ��: ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_AVPLAYERTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_AVPLAYERTEST);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

MPlayer player;
//
//   ����: InitInstance(HINSTANCE, int)
//
//   Ŀ��: ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hPlayerModule = LoadLibrary(L"AVPlayerCore.dll");
   if (hPlayerModule)
   {
	   player.m_funcInit = (funcInit)GetProcAddress(hPlayerModule, "InitPlayer");
	   player.m_funcUnInit = (funcUnInit)GetProcAddress(hPlayerModule, "UnInitPlayer");

	   player.m_funcCreatePlayer = (funcCreatePlayer)GetProcAddress(hPlayerModule, "CreateMPlayer");
	   player.m_funcDesotryPlayer = (funcDestoryPlayer)GetProcAddress(hPlayerModule, "DestoryMPlayer");
	   player.m_funcSetOptions = (funcSetOptions)(GetProcAddress)(hPlayerModule, "SetOptions");
	   player.m_funcPlay = (funcPlay)GetProcAddress(hPlayerModule, "Play");
	   player.m_funcStop = (funcStop)GetProcAddress(hPlayerModule, "Stop");
	   player.m_funcIsPlaying = (funcIsPlaying)GetProcAddress(hPlayerModule, "IsPlaying");
	   player.m_funcSetWindowSize = (funcSetWindowSize)GetProcAddress(hPlayerModule, "SetWindowSize");

	   player.m_funcSetPanoScale = (funcSetPanoScale)GetProcAddress(hPlayerModule, "SetPanoScale");
	   player.m_funcSetPanoRotate = (funcSetPanoRotate)GetProcAddress(hPlayerModule, "SetPanoRotate");
	   player.m_funcSetPanoScroll = (funcSetPanoScroll)GetProcAddress(hPlayerModule, "SetPanoScroll");

	   player.m_funcInit();
	   player.m_hPlayer = player.m_funcCreatePlayer();
	   //ShellExecute(NULL, L"Open", L"ffplay.exe", L"-i rtmp://playrtmp.simope.com:1935/live/548c9b76d2?liveID=100031607", L"E:\\MyProjects\\ffmpeg\\New-MPlayer\\3rd\\ffmpeg-3.4.2-win32-shared\\bin\\", SW_SHOW);
	   // m_funcPlay(m_hPlayer, "rtmp://playrtmp.simope.com:1935/live/548c9b76d2?liveID=100031607");

	   {
		   PLAYER_OPTS opts;
		   opts.hWnd = hWnd;
		   opts.video_type = NORMAL_TYPE;
		   {
			   opts.strPath = "E:\\music\\MV\\hb\\hb.mp4";
			   //player.m_funcPlay(player.m_hPlayer, "rtmp://live.hkstv.hk.lxdns.com/live/hks");
			   player.m_funcPlay(player.m_hPlayer, opts);
		   }
		   /*opts.video_type = PANORAMIC_TYPE;
		   {
			   opts.bEnableAudio = false;
			   opts.hInstance = hInstance;
			   opts.strPath = "rtmp://live.simope.com:1935/live/87682e62fc?liveID=100032461&accessCode=b06ec69f";
			   player.m_funcPlay(player.m_hPlayer, opts);
		   }*/
	   }

	   //ShellExecute(NULL, L"Open", L"ffplay.exe", L"-i lzx.mp4", L"E:\\MyProjects\\ffmpeg\\New-MPlayer\\3rd\\ffmpeg-3.4.2-win32-shared\\bin\\", SW_SHOW);
	   //m_funcPlay(m_hPlayer, "E:\\MyProjects\\ffmpeg\\New-MPlayer\\Debug\\lzx.mp4");
   }

   SetForegroundWindow(hWnd);
   SetActiveWindow(hWnd);

   return TRUE;
}


bool	m_bLButtonDown = false;
int		m_xPos = 0;
int		m_yPos = 0;
int		m_xCurrPos = 0;
int		m_yCurrPos = 0;
float	m_scale = 0.5f;

//
//  ����: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND  - ����Ӧ�ó���˵�
//  WM_PAINT    - ����������
//  WM_DESTROY  - �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �����˵�ѡ��: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: �ڴ˴����ʹ�� hdc ���κλ�ͼ����...
            EndPaint(hWnd, &ps);
        }
        break;

	case WM_LBUTTONDOWN:
	{
		m_bLButtonDown = true;
		SetCapture(hWnd);
		m_xPos = LOWORD(lParam);
		m_yPos = HIWORD(lParam);
		//m_funcSetPanoRotate(m_hPlayer, m_xPos * 5, m_yPos * 5);	
	}
	break;
	case WM_LBUTTONUP:
	{
		ReleaseCapture();
		m_bLButtonDown = false;
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (m_bLButtonDown)
		{
			int xPos = LOWORD(lParam);
			int yPos = HIWORD(lParam);
			m_xCurrPos += (xPos - m_xPos) * 5;
			m_yCurrPos += (yPos - m_yPos) * 5;
			player.m_funcSetPanoRotate(player.m_hPlayer, m_xCurrPos, m_yCurrPos);
			m_xPos = xPos;
			m_yPos = yPos;
		}
	}
	break;
	case WM_WTSSESSION_CHANGE:
	{
		OutputDebugString(std::to_wstring(wParam).c_str());
		break;
	}
	case WM_MOUSEWHEEL:
	{
		int fwKeys = GET_KEYSTATE_WPARAM(wParam);
		int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (zDelta < 0)
			m_scale += 0.1f;
		else
			m_scale -= 0.1f;

		if (m_scale < 0.5f)
			m_scale = 0.5f;

		if (m_scale > 1.5f)
			m_scale = 1.5f;

		player.m_funcSetPanoScale(player.m_hPlayer, m_scale);
		int xPos = LOWORD(lParam);
		int yPos = HIWORD(lParam);
		player.m_funcSetPanoRotate(player.m_hPlayer, m_xCurrPos, m_yCurrPos);
	}
	break;
	case WM_SIZE:
	{
		RECT rcWnd;
		::GetWindowRect(hWnd, &rcWnd);
		if (player.m_funcSetWindowSize && player.m_funcSetWindowSize != INVALID_HANDLE_VALUE)
		{
			player.m_funcSetWindowSize(player.m_hPlayer, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top);
		}
	}
	break;
    case WM_DESTROY:
		player.m_funcStop(player.m_hPlayer);
        //PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// �����ڡ������Ϣ�������
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
