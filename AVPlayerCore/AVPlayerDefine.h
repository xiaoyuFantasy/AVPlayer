#pragma once
#include <Windows.h>
#include <string>
#include <functional>
#include <algorithm>
#include <future>
using namespace std;

enum ErrorID
{
	OpenError,
	GlCreateError,
};

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
	PlayerRenderFrame,
	PlayerClosed,
	PlayerError,
};

struct PlayerOpenSt
{
	bool IsHasVideo = false;
	bool IsHasAudio = false;
	int duration;
};
struct PlayerFirstFrameSt
{
	int width;
	int height;
};
struct PlayerAudioStateSt
{
	int channels;
	int sample_rate;
};
struct PlayerVideoStateSt
{
	int nFixelFormat;
	int width;
	int height;
};
struct PlayerPositionSt
{
	double pos;
};
struct PlayerFrameSt
{
	int nPixelFormat;
	int nWidth;
	int nHeight;
	unsigned char** ppData;
};
struct PlayerErrorSt
{
	int nCode;
	std::string strErrMsg;
};

//typedef struct PLAYER_EVENT_T
//{
//	int type;
//	void *userData;
//	union
//	{
//		
//	};
//};

enum PLAY_STATUS
{
	NoneStatus,
	OpeningStatus,
	OpenedStatus, 
	PlayingStatus, 
	PausedStatus,
	CompletedStatus, 
	StopedStatus,
	//CloseStatus,
};

enum VIDEO_TYPE
{
	NORMAL_TYPE = 1 ,
	PANORAMIC_TYPE = 2,
};

enum DECODE_TYPE
{
	CPU_DECODE,
	GPU_DECODE,
};

enum RENDER_MODE {
	PANO2D = 0,		// 普通2D播放模式
	STANDARD = 1,	// 球形全景播放模式
	FISH_EYE = 2,	// 鱼眼播放模式
	STEREO = 3,		// 小行星播放模式
};

typedef void(*FuncPlayerEvent)(void *user_data, const PLAYER_EVENT e, void *pData);

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