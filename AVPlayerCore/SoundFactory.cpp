#include "stdafx.h"
#include "SoundFactory.h"
#include "SDLSound.h"

CSoundFactory::CSoundFactory()
{
}


CSoundFactory::~CSoundFactory()
{
}

ISound * CSoundFactory::CreateSound()
{
	return new CSDLSound();
}

void CSoundFactory::ReleaseSound(ISound ** sound)
{
	if (*sound)
	{
		delete *sound;
		*sound = nullptr;
	}
}
