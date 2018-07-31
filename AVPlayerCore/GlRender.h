#pragma once
#include "RenderDefine.h"
#include <glad\glad.h>
#pragma comment(lib, "glad.lib")

#include <glfw3.h>
#pragma comment(lib, "glfw3.lib")

class CGlRender : public IRender
{
public:
	CGlRender();
	virtual ~CGlRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd) override;
	void DestoryRender() override;
	bool SetRenderMode(RENDER_MODE) override;
	void SetRenderSize(int width, int height) override;
	void RenderFrameData(AVFrame *frame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

protected:
	static void resize_callback(GLFWwindow* window, int width, int height);

private:
	HWND m_hWnd = nullptr;
	GLFWwindow*	m_pWindow = nullptr;
};

