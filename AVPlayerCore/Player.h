#pragma once

class IPlayer
{
public:
	virtual void Play() = 0;
	virtual bool IsPlaying() = 0;
	virtual void Pause(bool IsPause = true) = 0;
	virtual bool IsPaused() = 0;
	virtual void Stop() = 0;
};
