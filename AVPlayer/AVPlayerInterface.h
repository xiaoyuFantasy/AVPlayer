#pragma once
#include <Windows.h>
#include <string>
#include "../AVPlayerCore/AVPlayerDefine.h"

//enum VIDEO_TYPE
//{
//	NORMAL_TYPE,
//	PANORAMIC_TYPE,
//};
//
//typedef void(*DurationCallback)(void *userdata, int64_t duration);
//typedef void(*ProgressCallback)(void *userdata, int64_t progress);
//typedef void(*VideoCallback)(void *userdata, int width, int height);
//typedef void(*ErrorCallback)(void *userdata, int err, std::string strMsg);
//
//typedef struct _PLAYER_OPTS
//{
//	void*		user_data = nullptr;
//	string		strPath;
//	HWND		hWnd = nullptr;
//	bool		bEnableVideo = true;
//	bool		bEnableAudio = true;
//	VIDEO_TYPE	video_type = NORMAL_TYPE;
//	HINSTANCE	hInstance;
//	DurationCallback durationCb = nullptr;
//	ProgressCallback progreeeCb = nullptr;
//	VideoCallback	videoCb = nullptr;
//	ErrorCallback errorCb = nullptr;
//} PLAYER_OPTS;

typedef void(*funcInit)();
typedef void(*funcUnInit)();
typedef HANDLE(*funcCreatePlayer)();
typedef void(*funcDestoryPlayer)(HANDLE handle);
typedef bool(*funcSetOptions)(HANDLE handle, PLAYER_OPTS &opts);
typedef bool(*funcPlay)(HANDLE handle, PLAYER_OPTS &opts);
typedef void(*funcStop)(HANDLE handle);
typedef bool(*funcIsPlaying)(HANDLE handle);
typedef void(*funcPause)(HANDLE handle, bool bPause);
typedef bool(*funcIsPaused)(HANDLE handle);
typedef bool(*funcSeek)(HANDLE handle, int nPos);
typedef void(*funcSetVolume)(HANDLE handle, int nVolume);
typedef void(*funcSetMuted)(HANDLE handle, bool bMuted);
typedef void(*funcSetWindowSize)(HANDLE handle, int nWidth, int nHeight);
typedef void(*funcSetPanoScale)(HANDLE handle, float factor);
typedef void(*funcSetPanoRotate)(HANDLE handle, float x, float y);
typedef void(*funcSetPanoScroll)(HANDLE handle, float latitude, float longitude);