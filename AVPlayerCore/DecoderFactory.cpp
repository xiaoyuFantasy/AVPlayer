#include "stdafx.h"
#include "DecoderFactory.h"
#include "FFmpegDecoder.h"


IDecoder * CDecoderFactory::CreateDecoder()
{
	return new CFFmpegDecoder();
}

void CDecoderFactory::ReleaseDecoder(IDecoder ** decoder)
{
	if (*decoder)
	{
		delete *decoder;
		*decoder = nullptr;
	}
}
