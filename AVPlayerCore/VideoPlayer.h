#pragma once
#include "VideoDefine.h"
#include "RenderDefine.h"
#include "DecoderDefine.h"
#include "AVPlayerDefine.h"
#include "ClockMgr.h"

#define MAX_VIDEO_SIZE (1024) //25 * 256 * 1024

class CVideoPlayer
{
public:
	CVideoPlayer(AVStream* pStream, IRender* render);
	virtual ~CVideoPlayer();

public:
	void SetClockMgr(CClockMgr* clockMgr = nullptr);
	bool Open(PLAYER_OPTS &opts);
	void Play();
	bool IsPlaying();
	void Pause(bool IsPause = true);
	bool IsPaused();
	void Stop();
	double GetClock();
	void ClearFrame();

public:
	void PushPacket(PacketPtr && packet_ptr);

protected:
	bool CreateDecoder();
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
	PacketQueue			m_queuePacket;
	FrameQueue			m_queueFrame;
	AVHWDeviceType		m_typeCodec = AV_HWDEVICE_TYPE_NONE;
	AVBufferRef*		m_pHWDeviceCtx = nullptr;
	AVPixelFormat		m_hwPixelFormat;
	double				m_clock = 0.0;

	std::thread			m_threadDecode;
	std::thread			m_threadRender;

	IRender*			m_pRender = nullptr;
	IDecoder*			m_pDecoder = nullptr;
	CClockMgr*			m_pClockMgr = nullptr;
	//ת��
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

