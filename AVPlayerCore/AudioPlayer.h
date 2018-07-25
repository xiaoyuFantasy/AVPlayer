#pragma once
#include <atomic>
#include <string>
#include "AudioDefine.h"
#include "AVPlayerDefine.h"
#include "Player.h"
#include "Clock.h"

#define AUDIO_MIN_BUFFER_SIZE 512
#define AUDIO_MAX_CALLBACKS_PER_SEC 30
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define MAX_AUDIO_SIZE (1024) //25 * 16 * 1024

class CAudioPlayer : public IPlayer, public IClock
{
public:
	CAudioPlayer(AVStream* pStream);
	virtual ~CAudioPlayer();

public:
	bool Open(PLAYER_OPTS &opts);
	void Play();
	bool IsPlaying();
	void Pause(bool IsPause = true);
	bool IsPaused();
	void Stop();
	double GetClock();
	void SetVolume(int nVolume);
	int GetVolume() const;
	void SetMuted(bool bMuted);
	bool IsMuted() const;
	void PushPacket(PacketPtr && packet_ptr);
	void ClearFrame();

protected:
	bool CreateDecoder();
	bool OpenAudioDevice();
	void DecodeThread();
	static void audio_callback(void* userData, Uint8* stream, int len);
	int GetFrame();
	int DecodeFrame(AVFrame* pFrame);

private:
	PLAYER_OPTS			m_opts;
	AVStream*			m_pStream = nullptr;
	AVCodecContext*		m_pCodecCtx = nullptr;
	std::atomic_bool	m_bQuit = false;
	std::atomic_bool	m_bOpen = false;
	std::atomic_bool	m_bPaused = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_int		m_nVolume = 60;
	std::atomic_bool	m_bMuted = false;
	PacketQueue			m_queuePacket;
	FrameQueue			m_queueFrame;
	int64_t				m_nCallbackTime;
	uint8_t				m_bufferTemp[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4] = { 0 };
	uint8_t				m_bufferSilence[1024] = { 0 };
	uint8_t*			m_buffer = nullptr;
	std::atomic_int		m_nBufferIndex = 0;
	std::atomic_int		m_nBufferSize = 0;
	AudioParams			m_hwParams; //Éù¿¨²ÎÊý
	AudioParams			m_hwSrc;
	SDL_AudioDeviceID	m_audioDevID;
	SwrContext*			m_pSwrCtx = nullptr;
	double				m_clock = 0.0;
	double				m_pts = 0.0;
	int					m_hwSize = 0;
	std::thread			m_threadDecode;
	std::atomic_bool	m_bStopDecode = false;
};

