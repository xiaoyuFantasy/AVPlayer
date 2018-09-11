#include "stdafx.h"
#include "GlRender.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

const GLuint WIDTH = 800, HEIGHT = 600;
const int pixel_w = 2160, pixel_h = 1080;
unsigned char buf[pixel_w*pixel_h * 3 / 2] = { 0 };
unsigned char *plane[3];
FILE *fileYuv = nullptr;

CGlRender::CGlRender()
{
}

CGlRender::~CGlRender()
{
	if (m_texture[0] != 0)
		glDeleteTextures(3, m_texture);
	if (m_glVAO != 0)
		glDeleteVertexArrays(1, &m_glVAO);
	if (m_glVertices != 0)
		glDeleteBuffers(1, &m_glVertices);
	if (m_glIndices != 0)
		glDeleteBuffers(1, &m_glIndices);
	if (m_glTexturecoords != 0)
		glDeleteBuffers(1, &m_glTexturecoords);

	if (m_hDC)
		ReleaseDC(m_hWnd, m_hDC);
}

bool CGlRender::InitRender()
{
	return true;
}

bool CGlRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	if (!::IsWindow(hWnd))
		return false;

	/*RECT rc;
	GetWindowRect(hWnd, &rc);
	std::string strWndName = CreateGuidToString("MPlayerWnd_");
	PANO_INFO info{ strWndName.c_str(), (char*)strWndName.c_str(), 1, nWidth, nHeight, 0, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, PLAY_MODE::PANO2D, FRAME_FORMAT::VIDEO };
	m_hNativeRender = NativeOnCreate(hWnd, nullptr, nullptr, &info);
	if (nullptr == m_hNativeRender)
	{
		av_log(NULL, AV_LOG_ERROR, "NativeOnCreate Failed!!!");
		return false;
	}*/

	m_hWnd = hWnd;
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
	m_nPixelFormat = pixelFormat;

	m_oldWndProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)mWndProc);
	
	if (!CreateHRC())
	{
		av_log(NULL, AV_LOG_ERROR, "CreateHRC Failed!!!");
		return false;
	}

	if (!CreateGL())
	{
		av_log(NULL, AV_LOG_ERROR, "CreateGL Failed!!!");
		return false;
	}

	if (!CreateSwsCtx())
	{
		av_log(NULL, AV_LOG_ERROR, "CreateSwsCtx Failed!!!");
		return false;
	}

	plane[0] = buf;
	plane[1] = plane[0] + pixel_w*pixel_h;
	plane[2] = plane[1] + pixel_w*pixel_h / 4;
	fileYuv = _fsopen("..\\Debug\\panor_2160x1080.yuv", "rb", _SH_DENYWR);

	std::thread([&]() {
		MSG msg;
		// 循环一直到接收WM_QUIT(0)消息
		while (::GetMessage(&msg, 0, 0, 0) > 0)
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}).detach();
	
	return true;
}

void CGlRender::DestoryRender()
{
	m_bClose = true;

	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

}

bool CGlRender::SetRenderMode(RENDER_MODE)
{
	return false;
}

void CGlRender::SetRenderSize(int width, int height)
{
	if (height == 0) 
		height = 1;

	m_nWndWidth = width;
	m_nWndHeight = height;
	glViewport(0, 0, m_nWndWidth, m_nWndHeight);
	/*if (m_hNativeRender != nullptr)
		NativeUpdateViewport(m_hNativeRender, 0, 0, m_nWndWidth, m_nWndHeight);*/
}

void CGlRender::RenderFrameData(FramePtr pFrame)
{
	if (m_bClose)
		return;

	int nHeight = sws_scale(
		m_pSwsCtx,
		(uint8_t const * const *)pFrame->data,
		pFrame->linesize,
		0,
		pFrame->height,
		m_pFrameOut->data,
		m_pFrameOut->linesize);

	/*if (m_hNativeRender != nullptr)
		NativeSetFrameData(m_hNativeRender, STREAM_FORMAT::FRAME_FORMAT_YUV, m_pFrameOut->data);*/
	if (fread(buf, 1, pixel_w*pixel_h * 3 / 2, fileYuv) != pixel_w*pixel_h * 3 / 2) {
		if (feof(fileYuv))
			fseek(fileYuv, 0, 0);
	}

	wglMakeCurrent(m_hDC, m_hRC);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w, pixel_h, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);
	m_pShader->setInt("tex_y", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_texture[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);
	m_pShader->setInt("tex_u", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_texture[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);
	m_pShader->setInt("tex_v", 2);

	m_pShader->use();
	glBindVertexArray(m_glVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	::SwapBuffers(m_hDC);

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

void CGlRender::InitGL()
{
}

bool CGlRender::CreateHRC()
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
	return true;
}

bool CGlRender::CreateGL()
{
	if (!gladLoadGL())
	{
		av_log(NULL, AV_LOG_ERROR, "glewInit Failed!!!");
		return false;
	}

	static Shader glShader("Shader.vsh", "Shader.fsh");
	m_pShader = &glShader;

	RECT rc;
	GetWindowRect(m_hWnd, &rc);
	glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);

	CreateVertices();
	CreateTexture();

	// note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
	// VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
	glBindVertexArray(0);

	return true;
}

void CGlRender::CreateVertices()
{
	float vertices[] = {
		// positions         
		1.0f,  1.0f, 0.0f,	
		1.0f, -1.0f, 0.0f,	
		-1.0f, -1.0f, 0.0f,	
		-1.0f,  1.0f, 0.0f,
	};

	float texturecoords[] = {
		// texture coords
		1.0f, 0.0f, // top right
		1.0f, 1.0f, // bottom right
		0.0f, 1.0f, // bottom left
		0.0f, 0.0f  // top left 
	};

	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};


	
	glGenVertexArrays(1, &m_glVAO);
	glGenBuffers(1, &m_glVertices);
	glGenBuffers(1, &m_glIndices);
	glGenBuffers(1, &m_glTexturecoords);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(m_glVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_glVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, m_glTexturecoords);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturecoords), texturecoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
}

void CGlRender::CreateTexture()
{
	glGenTextures(3, m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, m_texture[2]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
											  // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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

LRESULT CGlRender::mWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
