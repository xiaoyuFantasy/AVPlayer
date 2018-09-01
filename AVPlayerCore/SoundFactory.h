#pragma once
#include "SoundDefine.h"
#include "SingleModule.h"

class CSoundFactory : public CSingleModule<CSoundFactory>
{
public:
	CSoundFactory();
	virtual ~CSoundFactory();

	std::shared_ptr<ISound> CreateSound();
};

