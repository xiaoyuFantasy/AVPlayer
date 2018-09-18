#include "stdafx.h"
#include "GlRender.h"
#include <math.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <geometry.h>
#include <shlwapi.h>
#include <atlbase.h>
#include <atlstr.h>

wstring GetPlayerPath(LPCTSTR lpszFolderName = nullptr)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	PathRemoveFileSpec(szPath);
	PathAppend(szPath, L"shader");
	if (lpszFolderName)
	{
		PathAddBackslash(szPath);
		PathAppend(szPath, lpszFolderName);
	}
	PathAddBackslash(szPath);
	return szPath;
}


CGlRender::CGlRender()
	:m_camera(glm::vec3(0.0f, 0.0f, 0.0f))
{
}

CGlRender::~CGlRender()
{
	DestoryRender();
}

bool CGlRender::InitRender()
{
	return true;
}

bool CGlRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	if (!::IsWindow(hWnd))
		return false;

	m_hWnd = hWnd;
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
	m_nPixelFormat = pixelFormat;

	RECT rc;
	GetWindowRect(hWnd, &rc);
	m_nWndWidth = rc.right - rc.left;
	m_nWndHeight = rc.bottom - rc.top;

	if (!CreateSwsCtx())
		return false;

	if (!InitGL())
		return false;	
	return true;
}

void CGlRender::DestoryRender()
{
	if (m_pVideoBuffer)
	{
		av_free(m_pVideoBuffer);
		m_pVideoBuffer = nullptr;
	}

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	if (m_glTexture[0] != 0)
		glDeleteTextures(3, m_glTexture);
	if (m_glVAO != 0)
		glDeleteVertexArrays(1, &m_glVAO);
	if (m_glVertices != 0)
		glDeleteBuffers(1, &m_glVertices);
	if (m_glIndices != 0)
		glDeleteBuffers(1, &m_glIndices);
	if (m_glTexturecoords != 0)
		glDeleteBuffers(1, &m_glTexturecoords);

	m_vectVertices.clear();
	m_vectIndices.clear();
	m_vectTexcoords.clear();

	if (m_hDC != 0)
	{
		wglMakeCurrent(m_hDC, 0);
		if (m_hRC != 0)
		{
			wglDeleteContext(m_hRC);
			m_hRC = 0;
		}
		ReleaseDC(m_hWnd, m_hDC);
		m_hDC = 0;
	}
	m_hWnd = 0;
	if (m_hDC)
		ReleaseDC(m_hWnd, m_hDC);
}

bool CGlRender::SetRenderMode(RENDER_MODE mode)
{
	if (mode == m_renderMode)
		return true;

	//m_renderMode = mode;
	/*SetVerticesModel(mode);*/
	return false;
}

void CGlRender::SetRenderSize(int width, int height)
{
	if (height == 0) 
		height = 1;

	/*m_nWndWidth = width;
	m_nWndHeight = height;
	glViewport(0, 0, m_nWndWidth, m_nWndHeight);*/
}

void CGlRender::RenderFrameData(FramePtr pFrame)
{
	if (!::IsWindow(m_hWnd) || !IsWindowVisible(m_hWnd) || !pFrame)
		return;

	int nHeight = sws_scale(
		m_pSwsCtx,
		(uint8_t const * const *)pFrame->data,
		pFrame->linesize,
		0,
		pFrame->height,
		m_pFrameOut->data,
		m_pFrameOut->linesize);

	wglMakeCurrent(m_hDC, m_hRC);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_glTexture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth, m_nVideoHeight, 0, GL_RED, GL_UNSIGNED_BYTE, m_pFrameOut->data[0]);
	m_pShader->setInt("tex_y", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_glTexture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth / 2, m_nVideoHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_pFrameOut->data[1]);
	m_pShader->setInt("tex_u", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_glTexture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoWidth / 2, m_nVideoHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_pFrameOut->data[2]);
	m_pShader->setInt("tex_v", 2);

	m_pShader->use();

	if (m_renderMode == RENDER_MODE::PANO2D)
	{
		glm::mat4 projection(0.1f), view(0.1f), model(0.1f);
		m_pShader->setMat4("projection", projection);
		m_pShader->setMat4("view", view);
		m_pShader->setMat4("model", model);
	}
	else
	{
		glm::mat4 projection = glm::perspective(glm::radians(m_camera.Zoom), (float)m_nWndWidth / (float)m_nWndHeight, 0.1f, 100.0f);
		m_pShader->setMat4("projection", projection);
		glm::mat4 view = m_camera.GetViewMatrix();
		m_pShader->setMat4("view", view);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		float angle = 0.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_pShader->setMat4("model", model);
	}

	glBindVertexArray(m_glVAO);
	glDrawElements(GL_TRIANGLES, m_vectIndices.size(), GL_UNSIGNED_INT, 0);

	::SwapBuffers(m_hDC);

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;
	if (m_nWndWidth != width || m_nWndHeight != height)
	{
		glViewport(0, 0, width, height);
		m_nWndWidth = width;
		m_nWndHeight = height;
	}
}

void CGlRender::SetScale(float factor)
{
	m_camera.ProcessMouseScroll(factor);
}

void CGlRender::SetRotate(float x, float y)
{
	m_camera.ProcessMouseMovement(x, y);
}

void CGlRender::SetScroll(float latitude, float longitude)
{
	
}

void CGlRender::SetReverse(bool filp)
{
}

bool CGlRender::InitGL()
{
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_STEREO |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		16,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	m_hDC = GetDC(m_hWnd);
	if (m_hDC == 0)
		return false;
	GLuint PixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (PixelFormat == 0)
		return false;
	if (SetPixelFormat(m_hDC, PixelFormat, &pfd) == false)
		return false;
	m_hRC = wglCreateContext(m_hDC);
	if (m_hRC == 0)
		return false;
	if (wglMakeCurrent(m_hDC, m_hRC) == false)
		return false;

	if (!gladLoadGL())
		return false;

	std::wstring wstrVshPath = GetPlayerPath() + L"Shader.vsh";
	std::wstring wstrFshPath = GetPlayerPath() + L"Shader.fsh";
	//m_pShader = std::make_shared<Shader>("Shader.vsh", "Shader.fsh");
	m_pShader = std::make_shared<Shader>(CW2A(wstrVshPath.c_str(), CP_UTF8), CW2A(wstrFshPath.c_str(), CP_UTF8));

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);

	/*** vertices ***/
	glGenVertexArrays(1, &m_glVAO);
	glGenBuffers(1, &m_glVertices);
	glGenBuffers(1, &m_glIndices);
	glGenBuffers(1, &m_glTexturecoords);

	SetVerticesModel();

	/*** yuv texture ***/
	glGenTextures(3, m_glTexture);
	glBindTexture(GL_TEXTURE_2D, m_glTexture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
												// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_glTexture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
												// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_glTexture[2]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
												// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	return true;
}

void CGlRender::SetVerticesModel(RENDER_MODE mode)
{
	m_vectVertices.clear();
	m_vectIndices.clear();
	m_vectTexcoords.clear();
	if (mode == RENDER_MODE::PANO2D)
		generatePlaneGeometry(m_vectVertices, m_vectIndices, m_vectTexcoords);
	else
		generateSphereGeometry(2.0f, m_vectVertices, m_vectIndices, m_vectTexcoords);

	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(m_glVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_glVertices);
	glBufferData(GL_ARRAY_BUFFER, m_vectVertices.size() * sizeof(GLfloat), m_vectVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vectIndices.size() * sizeof(GLuint), m_vectIndices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_glTexturecoords);
	glBufferData(GL_ARRAY_BUFFER, m_vectTexcoords.size() * sizeof(GLfloat), m_vectTexcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

bool CGlRender::CreateSwsCtx()
{
	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	m_pFrameOut = av_frame_alloc();
	m_pFrameOut->format = AV_PIX_FMT_YUV420P;
	m_pFrameOut->width = m_nVideoWidth;
	m_pFrameOut->height = m_nVideoHeight;

	m_pSwsCtx = sws_getContext(
		m_nVideoWidth,
		m_nVideoHeight,
		(AVPixelFormat)m_nPixelFormat,
		m_pFrameOut->width,
		m_pFrameOut->height,
		(AVPixelFormat)m_pFrameOut->format,
		SWS_BILINEAR,
		nullptr,
		nullptr,
		nullptr);

	if (!m_pSwsCtx)
	{
		av_log(NULL, AV_LOG_ERROR, "sws_getContext failed.");
		DestoryRender();
		return false;
	}

	int nSize = av_image_get_buffer_size(
		(AVPixelFormat)m_pFrameOut->format,
		m_pFrameOut->width,
		m_pFrameOut->height,
		1);

	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);
	m_pVideoBuffer = (uint8_t*)av_malloc(nSize * sizeof(uint8_t));

	//设置帧数据对应的内存
	av_image_fill_arrays(
		m_pFrameOut->data,
		m_pFrameOut->linesize,
		m_pVideoBuffer,
		(AVPixelFormat)m_pFrameOut->format,
		m_pFrameOut->width,
		m_pFrameOut->height,
		1);

	return true;
}
