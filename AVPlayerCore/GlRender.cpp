#include "stdafx.h"
#include "GlRender.h"

CGlRender::CGlRender()
{
}


CGlRender::~CGlRender()
{
}

bool CGlRender::InitRender()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		av_log(NULL, AV_LOG_ERROR, "glad load gl procaddress failed!!!");
		return false;
	}
	return false;
}

bool CGlRender::CreateRender(HWND hWnd)
{
	if (!::IsWindow(hWnd))
		return false;
	
	RECT rc;
	GetWindowRect(hWnd, &rc);
	m_pWindow = glfwCreateWindow(rc.right - rc.left, rc.bottom - rc.top, "glwindow", nullptr, nullptr);
	if (!m_pWindow)
	{
		av_log(NULL, AV_LOG_ERROR, "create glwindow failed!!!");
		return false;
	}

	glfwSetFramebufferSizeCallback(m_pWindow, resize_callback);
	glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);

	m_hWnd = m_hWnd;
	return false;
}

void CGlRender::DestoryRender()
{
	if (!m_pWindow)
		return;

	glfwDestroyWindow(m_pWindow);
	m_pWindow = nullptr;
}

bool CGlRender::SetRenderMode(RENDER_MODE)
{
	return false;
}

void CGlRender::SetRenderSize(int width, int height)
{
	
}

void CGlRender::RenderFrameData(AVFrame *frame)
{
	
}

void CGlRender::SetScale(float factor)
{
}

void CGlRender::SetRotate(float x, float y)
{
}

void CGlRender::SetScroll(float latitude, float longitude)
{
}

void CGlRender::SetReverse(bool filp)
{
}

void CGlRender::resize_callback(GLFWwindow * window, int width, int height)
{
	glViewport(0, 0, width, height);
}
