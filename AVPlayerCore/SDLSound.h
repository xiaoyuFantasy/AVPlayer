#pragma once
#include "SoundDefine.h"

class CSDLSound : public ISound
{
public:
	CSDLSound();
	virtual ~CSDLSound();

	bool InitAudio() override;
	bool OpenAudio(void* userdata) override;
	bool CloseAudio() override;
	void SetCallback(funcCallback callback) override;
	void Start() override;
	void Pause() override;
	void SetVolume(int value) override;
	int GetVolume() override;

private:
	funcCallback		m_callback;
	SDL_AudioDeviceID	m_audioDevID;
};

