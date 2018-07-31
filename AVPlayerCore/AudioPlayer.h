#pragma once
#include <atomic>
#include <string>
#include "AudioDefine.h"
#include "AVPlayerDefine.h"
#include "SoundDefine.h"
#include "DecoderDefine.h"
#include "Player.h"
#include "ClockMgr.h"

#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio
#define MAX_AUDIO_SIZE (1024) //25 * 16 * 1024

class CAudioPlayer
{
public:
	CAudioPlayer(AVStream* pStream, ISound *pSound);
	virtual ~CAudioPlayer();

public:
	void SetClockMgr(CClockMgr* clockMgr = nullptr);
	bool Open(PLAYER_OPTS &opts);
	void Close();
	double GetClock();
	void PushPacket(PacketPtr && packet_ptr);
	void ClearFrame();

protected:
	bool CreateDecoder();
	int funcDecodeFrame(uint8_t** buffer);

private:
	PLAYER_OPTS			m_opts;
	AVStream*			m_pStream = nullptr;
	AVCodecContext*		m_pCodecCtx = nullptr;
	std::atomic_bool	m_bOpen = false;
	PacketQueue			m_queuePacket;
	int64_t				m_nCallbackTime;
	uint8_t				m_bufferTemp[AVCODEC_MAX_AUDIO_FRAME_SIZE * 4] = { 0 };
	uint8_t				m_bufferSilence[1024] = { 0 };

	SDL_AudioDeviceID	m_audioDevID;
	SwrContext*			m_pSwrCtx = nullptr;
	double				m_clock = 0.0;
	double				m_pts = 0.0;
	ISound*				m_pSound = nullptr;
	IDecoder*			m_pDecoder = nullptr;
	CClockMgr*			m_pClockMgr = nullptr;
};

