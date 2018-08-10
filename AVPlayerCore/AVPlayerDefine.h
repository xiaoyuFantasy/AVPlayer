#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include <algorithm>
#include <future>
using namespace std;

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

typedef void(*FuncPlayerEvent)(const PLAY_EVENT e, void *data);

typedef struct _PLAYER_OPTS
{
	void*		user_data = nullptr;
	string		strPath;
	HWND		hWnd = nullptr;
	bool		bEnableVideo = true;
	bool		bEnableAudio = true;
	VIDEO_TYPE	video_type = NORMAL_TYPE;
	FuncPlayerEvent	funcEvent = nullptr;
} PLAYER_OPTS;