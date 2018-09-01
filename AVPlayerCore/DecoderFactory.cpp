#include "stdafx.h"
#include "DecoderFactory.h"
#include "FFmpegDecoder.h"


std::shared_ptr<IDecoder> CDecoderFactory::CreateDecoder()
{
	return std::make_shared<CFFmpegDecoder>();
}
