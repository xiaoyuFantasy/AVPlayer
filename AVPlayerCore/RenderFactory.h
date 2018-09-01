#pragma once
#include "RenderDefine.h"
#include "SingleModule.h"

class CRenderFactory : public CSingleModule<CRenderFactory>
{
public:
	CRenderFactory();
	virtual ~CRenderFactory();

	std::shared_ptr<IRender> CreateRender(const std::string strRenderName = "");
};

