#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include <algorithm>
#include <future>
using namespace std;

enum PLAYER_EVENT
{
	PlayerOpening,
	PlayerFirstFrame,
	PlayerAudioState,
	PlayerVideoState,
	PlayerPlaying,
	PlayerPaused,
	PlayerStopped,
	PlayerForward,
	PlayerBackward,
	PlayerPositionChanged,
	PlayerClosed,
};

typedef struct PLAYER_EVENT_T
{
	int type;
	void *userData;
	union
	{
		struct PlayerOpen
		{
			bool IsHasVideo = false;
			bool IsHasAudio = false;
			int duration;
		};
		struct PlayerFirstFrame
		{
			int width;
			int height;
		};
		struct PlayerAudioState
		{
			int channels;
			int sample_rate;
		};
		struct PlayerVideoState
		{
			int nFixelFormat;
			int width;
			int height;
		};
		struct PlayerPosition
		{
			double pos;
		};
	};
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

enum DECODE_TYPE
{
	CPU_DECODE,
	GPU_DECODE,
};

typedef void(*FuncPlayerEvent)(const PLAYER_EVENT e, void *data);

typedef struct _PLAYER_OPTS
{
	void*		user_data = nullptr;
	string		strPath;
	HWND		hWnd = nullptr;
	bool		bEnableVideo = true;
	bool		bEnableAudio = true;
	VIDEO_TYPE	video_type = NORMAL_TYPE;
	bool		bGpuDecode = true;
	HINSTANCE	hInstance = nullptr;
	FuncPlayerEvent	funcEvent = nullptr;
} PLAYER_OPTS;