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
	HWND		m_hWnd = nullptr; // ���洰�ھ��.
	HGLRC		m_hglrc; // ������ɫ��������.
	HDC			m_hdc;  // OpenGL��Ⱦ��������.
	GLuint		m_texture;// ��ǰ��Ⱦ����.
	int			m_nVideoWidth;// ��Ƶ��.
	int			m_nVideoHeight;// ��Ƶ��.
	bool		m_bKeepAspect;// �Ƿ����ÿ�߱�.
	float		m_fWindowAspect;// ��߱�.
	RECT		m_rcLastClient;// ���λ�ò���.
	int			m_nWndWidth;// ��ǰ��.
	int			m_nWndHeight;// ��ǰ��.
	//ת��
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
};

