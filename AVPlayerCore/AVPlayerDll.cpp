#include "stdafx.h"
#include "AVPlayerDll.h"
#include "AVPlayer.h"

std::mutex mutexLog;
static HANDLE g_hCMD = nullptr;

void log_callback(void *p, int level, const char *format, va_list args)
{
	if (level == AV_LOG_ERROR || level == AV_LOG_INFO || level == AV_LOG_C(134))
	{
		mutexLog.lock();
		char szMessageBuffer[512] = { 0 };
		_vsnprintf(szMessageBuffer, 512, format, args);
		strcat(szMessageBuffer, "\n");
		if (g_hCMD)
		{
			DWORD num = 0;
			WriteConsoleA(g_hCMD, szMessageBuffer, strlen(szMessageBuffer), &num, NULL);
		}
		else
		{
			OutputDebugStringA(szMessageBuffer);
		}
		mutexLog.unlock();
	}
}

PLAYERCORE_API void InitPlayer()
{
	//初始化ffmpeg
	avformat_network_init();
	av_log_set_callback(log_callback);
	//初始化SDL_Video
	SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
}

PLAYERCORE_API void UnInitPlayer()
{
	av_log_set_callback(nullptr);
	OpenConsole(false);
	avformat_network_deinit();
	SDL_Quit();
}

HANDLE PLAYERCORE_API CreateMPlayer()
{
	CAVPlayer *player = new CAVPlayer;
	return player;
}

void PLAYERCORE_API DestoryMPlayer(HANDLE handle)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->Stop();
	delete player;
}

void PLAYERCORE_API SetVolume(HANDLE handle, int nVolume)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetVolume(nVolume);
}

int PLAYERCORE_API GetVolume(HANDLE handle)
{
	if (handle == nullptr)
		return 0;

	CAVPlayer *player = (CAVPlayer*)handle;
	return player->GetVolume();
}

void PLAYERCORE_API SetMuted(HANDLE handle, bool bMuted)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetMuted(bMuted);
}

bool PLAYERCORE_API IsMuted(HANDLE handle)
{
	if (handle == nullptr)
		return false;

	CAVPlayer *player = (CAVPlayer*)handle;
	return player->IsMuted();
}

//void PLAYERCORE_API SetOptions(HANDLE handle, PLAYER_OPTS & opts)
//{
//	if (handle == nullptr)
//		return;
//
//	/*MPlayer *player = (MPlayer*)handle;
//	player->SetOptions(opts);*/
//}

void PLAYERCORE_API SetPanoType(HANDLE handle, PLAY_MODE type)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetPanormaicType(type);
}

void PLAYERCORE_API SetPanoScale(HANDLE handle, float factor)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetPanoramicScale(factor);
}

void PLAYERCORE_API SetPanoRotate(HANDLE handle, float x, float y)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetPanoramicRotate(x, y);
}

void PLAYERCORE_API SetPanoScroll(HANDLE handle, float latitude, float longitude)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetPanoramicScroll(latitude, longitude);
}

bool PLAYERCORE_API Play(HANDLE handle, PLAYER_OPTS & opts)
{
	if (handle == nullptr)
		return false;

#ifdef _DEBUG
	OpenConsole();
#endif // DEBUG

	CAVPlayer *player = (CAVPlayer*)handle;
	player->Play(opts);
	return true;
}

bool PLAYERCORE_API Stop(HANDLE handle)
{
	if (handle == nullptr)
		return false;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->Stop();
	return true;
}

bool PLAYERCORE_API IsPlaying(HANDLE handle)
{
	if (handle == nullptr)
		return false;

	CAVPlayer *player = (CAVPlayer*)handle;
	return player->IsPlaying();
}

void PLAYERCORE_API Pause(HANDLE handle, bool bPause)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->Pause(bPause);
}

bool PLAYERCORE_API IsPaused(HANDLE handle)
{
	if (handle == nullptr)
		return false;

	CAVPlayer *player = (CAVPlayer*)handle;
	return player->IsPaused();
}

void PLAYERCORE_API Seek(HANDLE handle, int nPos)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->Seek(nPos);
}

void PLAYERCORE_API SetWindowSize(HANDLE handle, int nWidth, int nHeight)
{
	if (handle == nullptr)
		return;

	CAVPlayer *player = (CAVPlayer*)handle;
	player->SetWindowSize(nWidth, nHeight);
}

void PLAYERCORE_API OpenConsole(bool bOpen)
{
	if (bOpen)
	{
		if (!g_hCMD)
		{
			AllocConsole();
			g_hCMD = GetStdHandle(STD_OUTPUT_HANDLE);
		}
	}
	else
	{
		FreeConsole();
		g_hCMD = nullptr;
	}
}