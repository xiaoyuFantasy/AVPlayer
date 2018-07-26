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
	void SetCallback(funcCallback callback) override;
	void Start() override;
	void Pause() override;
	void SetVolume(int value) override;
	int GetVolume() override;

protected:
	static void audio_callback(void* userData, Uint8* stream, int len);

private:
	int		m_sampleRate;
	int		m_channels;
	int		m_channelLayout;
	funcCallback		m_callback;
	SDL_AudioDeviceID	m_audioDevID;
};

