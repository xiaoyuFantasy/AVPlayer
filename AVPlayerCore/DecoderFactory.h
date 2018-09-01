#pragma once
#include "DecoderDefine.h"
#include "SingleModule.h"

class CDecoderFactory : public CSingleModule<CDecoderFactory>
{
public:
	CDecoderFactory() = default;
	virtual ~CDecoderFactory() = default;

	std::shared_ptr<IDecoder> CreateDecoder();
};

