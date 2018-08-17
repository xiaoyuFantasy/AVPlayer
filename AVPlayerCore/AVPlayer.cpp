#include "stdafx.h"
#include "AVPlayer.h"
#include "AVPlayerImpl.h"

std::mutex mutexLog;
static HANDLE g_hCMD = nullptr;

void log_callback(void *p, int level, const char *format, va_list args)
{
	if (level == AV_LOG_ERROR || level == AV_LOG_INFO || level == AV_LOG_C(134))
	{
		mutexLog.lock();
		char szMessageBuffer[256] = { 0 };
		_vsnprintf(szMessageBuffer, 255, format, args);
		strcat(szMessageBuffer, "\n");
		if (g_hCMD)
		{
			DWORD num = 0;
			WriteConsoleA(g_hCMD, szMessageBuffer, strlen(szMessageBuffer), &num, NULL);
		}
		else
			OutputDebugStringA(szMessageBuffer);
		mutexLog.unlock();
	}
}

EXPORT_API void  OpenConsole(bool bOpen)
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

EXPORT_API void InitPlayer()
{
#ifdef _DEBUG
	OpenConsole(true);
#endif // _DEBUG
	av_log_set_callback(log_callback);
}

EXPORT_API void UnInitPlayer()
{
	
}

EXPORT_API HANDLE CreateAVPlayer()
{
	CAVPlayerImpl *player = new CAVPlayerImpl();
	return player;
}

EXPORT_API void DestoryAVPlayer(HANDLE handle)
{
	if (handle)
		delete handle;
}

EXPORT_API bool Open(HANDLE handle, PLAYER_OPTS & opts, bool bSync)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->Open(opts, bSync);
}

EXPORT_API bool Play(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->Play();
}

EXPORT_API void Pause(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Pause();
}

EXPORT_API void Resume(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Resume();
}

EXPORT_API void Stop(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Stop();
}

PLAY_STATUS Status(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->Status();
}

EXPORT_API bool WaitForCompletion(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->WaitForCompletion();
}

EXPORT_API void Close(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Close();
}

EXPORT_API void SeekTo(HANDLE handle, double fact)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->SeekTo(fact);
}

EXPORT_API void Volume(HANDLE handle, int nVolume)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Volume(nVolume);
}

EXPORT_API void Mute(HANDLE handle, bool s)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->Mute(s);
}

EXPORT_API void FullScreen(HANDLE handle, bool bfull)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->FullScreen(bfull);
}

EXPORT_API double CurrentPlayTime(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->CurrentPlayTime();
}

EXPORT_API double Duration(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->Duration();
}

EXPORT_API void VideoSize(HANDLE handle, int width, int height)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	player->VideoSize(width, height);
}

EXPORT_API double Buffering(HANDLE handle)
{
	CAVPlayerImpl *player = (CAVPlayerImpl *)handle;
	return player->Buffering();
}
