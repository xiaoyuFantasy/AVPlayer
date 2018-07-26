#pragma once
#include <functional>

using funcCallback = std::function<bool(uint8_t* buf, int len)>;

class ISound
{
public:
	ISound() = default;
	virtual ~ISound() = default;

	virtual bool InitAudio() = 0;
	virtual bool OpenAudio(int sample_rate, int channel, int channel_layout) = 0;
	virtual bool CloseAudio() = 0;
	virtual void SetCallback(funcCallback callback) = 0;
	virtual void Start() = 0;
	virtual void Pause() = 0;
	virtual void SetVolume(int value) = 0;
	virtual int GetVolume() = 0;
};