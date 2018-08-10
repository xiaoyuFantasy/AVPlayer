#pragma once
#include "RenderDefine.h"
#include <gl\GL.h>
#include <gl\GLU.h>
#pragma comment(lib, "Opengl32.lib")

//#include <glad\glad.h>
//#pragma comment(lib, "glad.lib")
//
//#include <glfw3.h>
//#pragma comment(lib, "glfw3.lib")

class CGlRender : public IRender
{
public:
	CGlRender();
	virtual ~CGlRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat = AV_PIX_FMT_YUV420P) override;
	void DestoryRender() override;
	bool SetRenderMode(RENDER_MODE) override;
	void SetRenderSize(int width, int height) override;
	void RenderFrameData(AVFrame *frame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

protected:
	void InitGL();
	//static void resize_callback(GLFWwindow* window, int width, int height);

private:
	std::atomic_bool m_bClose = false;
	//GLFWwindow* m_pWindow = nullptr; //gl
	HWND		m_hWnd = nullptr; // 保存窗口句柄.
	HGLRC		m_hglrc; // 窗口着色描述表句柄.
	HDC			m_hdc;  // OpenGL渲染描述表句柄.
	GLuint		m_texture;// 当前渲染纹理.
	int			m_nVideoWidth;// 视频宽.
	int			m_nVideoHeight;// 视频高.
	bool		m_bKeepAspect;// 是否启用宽高比.
	float		m_fWindowAspect;// 宽高比.
	RECT		m_rcLastClient;// 最后位置参数.
	int			m_nWndWidth;// 当前宽.
	int			m_nWndHeight;// 当前高.
	//转换
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

