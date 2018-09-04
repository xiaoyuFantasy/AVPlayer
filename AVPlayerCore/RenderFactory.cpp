#include "stdafx.h"
#include "RenderFactory.h"
#include "SDLRender.h"
#include "GlRender.h"
#include "D3DRender.h"


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
	else if (strRenderName.compare("d3d") == 0)
		return std::make_shared<CD3DRender>();
	return std::make_shared<CSDLRender>();
}