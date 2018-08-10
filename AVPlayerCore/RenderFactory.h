#pragma once
#include "RenderDefine.h"
#include "SingleModule.h"

class CRenderFactory : public CSingleModule<CRenderFactory>
{
public:
	CRenderFactory();
	virtual ~CRenderFactory();

	IRender *CreateRender(const std::string strRenderName = "");
	void ReleaseRender(IRender** render);
};

