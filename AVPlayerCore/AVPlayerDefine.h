#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include <algorithm>
#include <future>


using namespace std;

typedef void(*callback_event)(const struct event_st *pevent, void *p_data);

enum PLAY_EVENT
{
	PlayerOpening,
	PlayerBuffering,
	PlayerPlaying,
	PlayerPaused,
	PlayerStopped,
	PlayerForward,
	PlayerBackward,
	PlayerPositionChanged,
	PlayerClosed,
};

enum PLAY_STATUS
{
	NoneStatus,
	OpenedStatus, 
	PlayingStatus, 
	PausedStatus,
	CompletedStatus, 
	StopedStatus,
	CloseStatus,
};


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