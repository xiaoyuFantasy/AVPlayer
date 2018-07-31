#pragma once
#include "RenderDefine.h"
#include "SingleModule.h"

class CRenderFactory : public CSingleModule<CRenderFactory>
{
public:
	CRenderFactory();
	virtual ~CRenderFactory();

	IRender *CreateRender();
	void ReleaseRender(IRender** render);
};

