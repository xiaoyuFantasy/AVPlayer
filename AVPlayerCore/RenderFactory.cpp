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

std::shared_ptr<IRender> CRenderFactory::CreateRender(const std::string strRenderName)
{
	if (strRenderName.compare("opengl") == 0)
		return std::make_shared<CGlRender>();
	
	return std::make_shared<CSDLRender>();
}