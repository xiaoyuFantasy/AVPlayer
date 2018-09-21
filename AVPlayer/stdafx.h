// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <atlbase.h>
#include <atlstr.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
//���duilib֧��
#include "../DuiLib/UIlib.h"
using namespace DuiLib;
#ifdef _DEBUG
#   pragma comment(lib, "..\\Debug\\DuiLib_d.lib")
#else
#   pragma comment(lib, "..\\Release\\DuiLib.lib")
#endif

#define PLAYER_MSG_DURATION (WM_USER + 1)
#define PLAYER_MSG_PROGRESS (WM_USER + 2)
#define PLAYER_MSG_PLAY		(WM_USER + 3)
#define PLAYER_MSG_OPEN		(WM_USER + 4)
#define PLAYER_MSG_ERROR	(WM_USER + 5)
#define PLAYER_MSG_CLOSE	(WM_USER + 6)
#define PLAYER_MSG_SELECT	(WM_USER + 7)