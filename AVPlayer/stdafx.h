// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <atlbase.h>
#include <atlstr.h>


// TODO:  在此处引用程序需要的其他头文件
//添加duilib支持
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