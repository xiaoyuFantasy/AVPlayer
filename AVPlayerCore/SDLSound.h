#pragma once
#include "SoundDefine.h"

#define SDL_AUDIO_BUFFER_SIZE 1024
#define AVCODEC_MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

class CSDLSound : public ISound
{
public:
	CSDLSound();
	virtual ~CSDLSound();

	bool InitAudio() override;
	bool OpenAudio(int sample_rate, int channel, int channel_layout) override;
	bool CloseAudio() override;
	void SetCallback(funcDecodeFrame callback) override;
	void Start() override;
	void Pause(bool bPause) override;
	void SetVolume(int value) override;
	int GetVolume() override;
	int GetWantedSampleRate() override;
	int GetWantedChannels() override;
	int GetWantedChannelLayout() override;

protected:
	static void audio_callback(void* userData, Uint8* stream, int len);

private:
	int		m_sampleRate;
	int		m_channels;
	int		m_channelLayout;
	std::atomic_bool	m_bOpen = false;
	std::atomic_bool	m_bPlaying = false;
	std::atomic_int		m_nVolume = 60;
	int					m_nBufferSize = 0;
	int					m_nBufferIndex = 0;
	uint8_t*			m_pBuffer = nullptr;
	funcDecodeFrame		m_funcDecodeFrame;
	SDL_AudioDeviceID	m_audioDevID;
};

