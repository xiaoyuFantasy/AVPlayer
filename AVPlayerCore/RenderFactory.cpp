#include "stdafx.h"
#include "RenderFactory.h"
#include "SDLRender.h"
#include "GlRender.h"


CRenderFactory::CRenderFactory()
{
}


CRenderFactory::~CRenderFactory()
{
}

IRender * CRenderFactory::CreateRender()
{
	return new CSDLRender();
}

void CRenderFactory::ReleaseRender(IRender ** render)
{
	if (*render)
	{
		(*render)->DestoryRender();
		delete *render;
		*render = nullptr;
	}
}
