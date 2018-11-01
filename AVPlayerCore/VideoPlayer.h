#pragma once
#include "VideoDefine.h"
#include "RenderDefine.h"
#include "DecoderDefine.h"
#include "AVPlayerDefine.h"
#include "ClockMgr.h"
#include "Dxva2Decoder.h"
#include <d3d9.h>
#pragma comment(lib, "D3D9.lib")

#define MAX_VIDEO_SIZE (256) //25 * 256 * 1024

class CVideoPlayer
{
public:
	CVideoPlayer();
	virtual ~CVideoPlayer();

public:
	void SetRender(std::shared_ptr<IRender> &render);
	void SetStream(AVStream* pStream);
	void SetClockMgr(CClockMgr* clockMgr);
	bool Open(PLAYER_OPTS &opts);
	void Play();
	bool IsPlaying();
	void Pause(bool IsPause = true);
	bool IsPaused();
	void Stop();
	double GetClock();
	void ClearFrame();
	void Reset();

public:
	void PushPacket(PacketPtr && packet_ptr);

protected:
	bool CreateDecoder();
	void DecodeThread();
	void RenderThread();
	bool CreateRender();
	void RenderFrame(AVFrame *pFrame);
	void HwRenderFrame(AVFrame *pFrame);
	double SyncVideo(AVFrame *frame, double pts);
	float SmoothVideo(AVFrame* frame, int size);

	static AVPixelFormat GetHwFormat(AVCodecContext *s, const AVPixelFormat *pix_fmts);

private:
	PLAYER_OPTS			m_opts;
	AVStream*			m_pStream = nullptr;
	AVCodecContext*		m_pCodecCtx = nullptr;
	std::atomic_int		m_nWndWidth;
	std::atomic_int		m_nWndHeight;
	double				m_dVideoProportion;
	std::atomic_bool	m_bClear = false;
	std::mutex			m_mxClearWait;
	std::condition_variable		m_cvClear;
	std::atomic_bool	m_bQuit = false;
	std::atomic_bool	m_bOpen = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_bool	m_bPause = false;
	std::mutex			m_mxPauseWait;
	std::condition_variable		m_cvPause;
	std::atomic_bool	m_bStopDecode = false;
	std::atomic_bool	m_bStopRender = false;
	PacketQueue			m_queuePacket;
	FrameQueue			m_queueFrame;
	AVHWDeviceType		m_typeCodec = AV_HWDEVICE_TYPE_NONE;
	AVBufferRef*		m_pHWDeviceCtx = nullptr;
	IDirect3DSurface9*	m_pDirect3DSurfaceRender = NULL;

	AVPixelFormat		m_hwPixelFormat;
	double				m_clock = 0.0;
	bool				m_bSetMulitSamples = false;
	std::thread			m_threadDecode;
	std::thread			m_threadRender;
	std::shared_ptr<IRender> m_pRender = nullptr;
	std::shared_ptr<IDecoder> m_pDecoder = nullptr;
	CClockMgr*			m_pClockMgr = nullptr;
	//hw render
	IDirect3D9              *m_pDirect3D9 = nullptr;
	IDirect3DDevice9        *m_pDirect3DDevice = nullptr;
};