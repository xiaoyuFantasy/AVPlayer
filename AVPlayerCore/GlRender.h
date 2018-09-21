#pragma once
#include "RenderDefine.h"
#include <shader.h>
#include <camera.h>
#ifdef GLAD
#include <glad/glad.h>
#pragma comment(lib, "glad.lib")
#endif // GLAD

#ifdef GLEW
#include <GL\glew.h>
#endif // GLEW
#pragma comment(lib, "Opengl32.lib")


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
	void RenderFrameData(AVFrame *pFrame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

protected:
	bool InitGL();
	void SetVerticesModel(RENDER_MODE mode = PANO2D);
	bool CreateSwsCtx();

private:
	int			m_nVideoWidth;// 视频宽.
	int			m_nVideoHeight;// 视频高.
	int			m_nWndWidth; //窗口宽
	int			m_nWndHeight; //窗口高
	bool		m_bKeepAspect;// 是否启用宽高比.
	float		m_fWindowAspect;// 宽高比.
	RECT		m_rcLastClient;// 最后位置参数.
	RENDER_MODE	m_renderMode = RENDER_MODE::PANO2D;
	WNDPROC		m_oldWndProc;
	//opengl
	HWND		m_hWnd = nullptr; // 保存窗口句柄.
	HDC			m_hDC;
	HGLRC		m_hRC;
	std::shared_ptr<Shader>	m_pShader;
	unsigned int m_glVAO = 0;
	unsigned int m_glVertices = 0;
	unsigned int m_glIndices = 0;
	unsigned int m_glTexturecoords = 0;
	unsigned int m_glTexture[3] = { 0 };
	std::vector<GLfloat> m_vectVertices;
	std::vector<GLuint> m_vectIndices;
	std::vector<GLfloat> m_vectTexcoords;
	// camera
	Camera m_camera;
	float m_lastX = 800.0f / 2.0;
	float m_lastY = 600.0 / 2.0;
	bool m_firstMouse = true;
	//转换
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
	HWND				m_hNativeRender = nullptr;
};

