#include "stdafx.h"
#include "SoundFactory.h"
#include "SDLSound.h"

CSoundFactory::CSoundFactory()
{
}


CSoundFactory::~CSoundFactory()
{
}

std::shared_ptr<ISound> CSoundFactory::CreateSound()
{
	return std::make_shared<CSDLSound>();
}
