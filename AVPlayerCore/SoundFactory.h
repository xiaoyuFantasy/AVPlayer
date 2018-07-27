#pragma once
#include "SoundDefine.h"
#include "SingleModule.h"

class CSoundFactory : public CSingleModule<CSoundFactory>
{
public:
	CSoundFactory();
	virtual ~CSoundFactory();

	ISound* CreateSound();
	void ReleaseSound(ISound **sound);
};

