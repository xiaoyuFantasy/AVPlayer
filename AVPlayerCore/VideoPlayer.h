#pragma once
#include <atomic>
#include <string>
#include <mutex>
#include <thread>
#include "VideoDefine.h"
#include "AVPlayerDefine.h"
#include "Player.h"
#include "Clock.h"
#include "CSharpInterface.h"

#define MAX_VIDEO_SIZE (1024) //25 * 256 * 1024

class CVideoPlayer : public IPlayer, public IClock
{
public:
	CVideoPlayer(AVStream* pStream);
	virtual ~CVideoPlayer();

public:
	bool Open(PLAYER_OPTS &opts);
	void SetAudioClock(IClock* pClock);
	void Play();
	bool IsPlaying();
	void Pause(bool IsPause = true);
	bool IsPaused();
	void Stop();
	double GetClock();
	void ClearFrame();

public:
	void SetPanormaicType(PLAY_MODE type);
	void SetPanoramicScale(float factor);
	void SetPanoramicRotate(float x, float y);
	void SetPanoramicScroll(float latitude, float longitude);
	void SetWindowSize(int nWidth, int nHeight);
	void PushPacket(PacketPtr && packet_ptr);

protected:
	bool CreateDecoder();
	bool CreateSDLWindow();
	void DecodeThread();
	void RenderThread();

	double SyncVideo(AVFrame *frame, double pts);
	float SmoothVideo(AVFrame* frame, int size);

private:
	PLAYER_OPTS			m_opts;
	AVStream*			m_pStream = nullptr;
	AVCodecContext*		m_pCodecCtx = nullptr;
	std::atomic_int		m_nWndWidth;
	std::atomic_int		m_nWndHeight;
	double				m_dVideoProportion;
	std::atomic_bool	m_bQuit = false;
	std::atomic_bool	m_bOpen = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_bool	m_bPaused = false;
	std::atomic_bool	m_bStopDecode = false;
	std::atomic_bool	m_bStopRender = false;
	//»´æ∞‰÷»æ
	HWND				m_hPanoramicWnd = nullptr;
	//SDL‰÷»æ
	SDL_Window*			m_pWindow = nullptr;
	SDL_Renderer*		m_pRenderer = nullptr;
	SDL_Texture*		m_pTexture = nullptr;
	//GDI+‰÷»æ
	std::mutex			m_mutexTexture;
	PacketQueue			m_queuePacket;
	FrameQueue			m_queueFrame;
	AVHWDeviceType		m_typeCodec = AV_HWDEVICE_TYPE_NONE;
	AVBufferRef*		m_pHWDeviceCtx = nullptr;
	AVPixelFormat		m_hwPixelFormat;
	double				m_clock = 0.0;

	std::thread			m_threadDecode;
	std::thread			m_threadRender;
	IClock*				m_pAudioClock = nullptr;

	//◊™ªª
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

