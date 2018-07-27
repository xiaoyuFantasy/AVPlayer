#pragma once
#include <functional>

using funcDecodeFrame = std::function<int(uint8_t** buf)>;

class ISound
{
public:
	virtual bool InitAudio() = 0;
	virtual bool OpenAudio(int sample_rate, int channel, int channel_layout) = 0;
	virtual bool CloseAudio() = 0;
	virtual void SetCallback(funcDecodeFrame callback) = 0;
	virtual void Start() = 0;
	virtual void Pause() = 0;
	virtual void SetVolume(int value) = 0;
	virtual int GetVolume() = 0;
};