#pragma once
#include <Windows.h>
#include <string>
#include <functional>
using namespace std;

enum VIDEO_TYPE
{
	NORMAL_TYPE,
	PANORAMIC_TYPE,
};

typedef void(*DurationCallback)(void *userdata, int64_t duration);
typedef void(*ProgressCallback)(void *userdata, int64_t progress);
typedef void(*VideoCallback)(void *userdata, int width, int height);
typedef void(*ErrorCallback)(void *userdata, int err, std::string strMsg);

typedef struct _PLAYER_OPTS
{
	void*		user_data = nullptr;
	string		strPath;
	HWND		hWnd = nullptr;
	bool		bEnableVideo = true;
	bool		bEnableAudio = true;
	VIDEO_TYPE	video_type = NORMAL_TYPE;
	HINSTANCE	hInstance;
	DurationCallback durationCb = nullptr;
	ProgressCallback progreeeCb = nullptr;
	VideoCallback	videoCb = nullptr;
	ErrorCallback errorCb = nullptr;
} PLAYER_OPTS;