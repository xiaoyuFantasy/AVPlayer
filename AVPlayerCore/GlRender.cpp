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
	/*glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		av_log(NULL, AV_LOG_ERROR, "glad load gl procaddress failed!!!");
		return false;
	}*/
	//int ret = gladLoadGL();
	return false;
}

bool CGlRender::CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat)
{
	if (!::IsWindow(hWnd))
		return false;

	m_hWnd = hWnd;
	m_nVideoWidth = nWidth;
	m_nVideoHeight = nHeight;
	m_nPixelFormat = pixelFormat;

	static PIXELFORMATDESCRIPTOR pfd =  // pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),	// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |					// Format Must Support Window
		PFD_SUPPORT_OPENGL |					// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,						// Must Support Double Buffering
		PFD_TYPE_RGBA,                   // Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,						// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,								// Accumulation Bits Ignored
		16,										// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,						// Main Drawing Layer
		0,											// Reserved
		0, 0, 0									// Layer Masks Ignored
	};

	if (!(m_hdc = GetDC(m_hWnd)))
	{
		av_log(nullptr, AV_LOG_ERROR, "Can't Create A GL Device Context.");
		return false;
	}

	GLuint PixelFormat;
	if (!(PixelFormat = ChoosePixelFormat(m_hdc, &pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		av_log(nullptr, AV_LOG_ERROR, "Can't Find A Suitable PixelFormat.");
		return false;
	}

	if (!SetPixelFormat(m_hdc, PixelFormat, &pfd))        // Are We Able To Set The Pixel Format?
	{
		av_log(nullptr, AV_LOG_ERROR, "Can't Set The PixelFormat.");
		return false;
	}

	if (!(m_hglrc = wglCreateContext(m_hdc)))  // Are We Able To Get A Rendering Context?
	{
		av_log(nullptr, AV_LOG_ERROR, "Can't Create A GL Rendering Context.");
		return false;
	}

	if (!wglMakeCurrent(m_hdc, m_hglrc))        // Try To Activate The Rendering Context
	{
		av_log(nullptr, AV_LOG_ERROR, "Can't Activate The GL Rendering Context.");
		return false;
	}

	RECT rc = { 0 };
	GetClientRect(m_hWnd, &rc);
	SetRenderSize(rc.right - rc.left, rc.bottom - rc.top);

	InitGL();

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	m_pFrameOut = av_frame_alloc();
	m_pFrameOut->format = AV_PIX_FMT_RGB24;
	m_pFrameOut->width = m_nVideoWidth;
	m_pFrameOut->height = m_nVideoHeight;

	m_pSwsCtx = sws_getContext(
		m_nVideoWidth,
		m_nVideoHeight,
		(AVPixelFormat)pixelFormat,
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

void CGlRender::DestoryRender()
{
	m_bClose = true;

	if (m_pVideoBuffer)
		av_free(m_pVideoBuffer);

	if (m_pFrameOut)
		av_frame_free(&m_pFrameOut);

	if (m_hglrc)                                       // Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL, NULL))					   // Are We Able To Release The DC And RC Contexts?
			av_log(nullptr, AV_LOG_ERROR, "Release Of DC And RC Failed.");

		if (!wglDeleteContext(m_hglrc))                 // Are We Able To Delete The RC?
			av_log(nullptr, AV_LOG_ERROR, "Release Rendering Context Failed.");

		m_hglrc = NULL;                                 // Set RC To NULL
	}

	if (m_hdc && !ReleaseDC(m_hWnd, m_hdc))            // Are We Able To Release The DC
		av_log(nullptr, AV_LOG_ERROR, "Release Device Context Failed.");
	
	m_hdc = NULL;
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
}

void CGlRender::RenderFrameData(AVFrame *frame)
{
	if (m_bClose)
		return;

	int nHeight = sws_scale(
		m_pSwsCtx,
		(uint8_t const * const *)frame->data,
		frame->linesize,
		0,
		frame->height,
		m_pFrameOut->data,
		m_pFrameOut->linesize);

	int width = 0;
	int height = 0;
	RECT rect_client;
	GetClientRect(m_hWnd, &rect_client);

	if (m_bKeepAspect)
	{
		int win_width = width = rect_client.right - rect_client.left;
		int win_height = height = rect_client.bottom - rect_client.top;
		int tmpheight = ((float)width / m_fWindowAspect);

		tmpheight += tmpheight % 2;
		if (tmpheight > height)
		{
			width = ((float)height * m_fWindowAspect);
			width += width % 2;
		}
		else
		{
			height = tmpheight;
		}

		// 居中对齐.
		rect_client.left += ((win_width - width) / 2);
		rect_client.top += ((win_height - height) / 2);
		rect_client.bottom -= ((win_height - height) / 2);
		rect_client.right -= ((win_width - width) / 2);

		if (!EqualRect(&m_rcLastClient, &rect_client))
		{
			m_rcLastClient = rect_client;

			glViewport(rect_client.left, rect_client.top, width, height);             // Reset The Current Viewport
			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									   // Reset The Projection Matrix
			// Calculate The Aspect Ratio Of The Window
			// GLfloat wh = (GLfloat)m_width / (GLfloat)m_height;
			// gluPerspective(120.0f, wh < 1.0f ? 1.0f : wh, 0.0f, 100.0f);
			glMatrixMode(GL_MODELVIEW);                  // Select The Modelview Matrix
			glLoadIdentity();                            // Reset The Modelview Matrix
		}
	}
	else
	{
		width = rect_client.right - rect_client.left;
		height = rect_client.bottom - rect_client.top;

		if (!EqualRect(&m_rcLastClient, &rect_client))
		{
			m_rcLastClient = rect_client;

			glViewport(rect_client.left, rect_client.top, width, height);             // Reset The Current Viewport

			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									   // Reset The Projection Matrix
			// Calculate The Aspect Ratio Of The Window
			// GLfloat wh = (GLfloat)m_width / (GLfloat)m_height;
			// gluPerspective(120.0f, wh < 1.0f ? 1.0f : wh, 0.0f, 100.0f);
			glMatrixMode(GL_MODELVIEW);                  // Select The Modelview Matrix
			glLoadIdentity();                            // Reset The Modelview Matrix
		}
	}

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, m_nWndWidth, m_nWndHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_pFrameOut->data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	glLoadIdentity();									// Reset The View
	glTranslatef(0.0f, 0.0f, -0.4f);
	glRotatef(180.0f, 1.0f, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, -1.0);
	glTexCoord2d(1.0, 0.0); glVertex2d(+1.0, -1.0);
	glTexCoord2d(1.0, 1.0); glVertex2d(+1.0, +1.0);
	glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, +1.0);
	glEnd();
	glFlush();

	glDeleteTextures(1, &m_texture);
	SwapBuffers(m_hdc);
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
	glEnable(GL_TEXTURE_2D);							// Enable Texture Mapping ( NEW )
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);        // Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
}

//void CGlRender::resize_callback(GLFWwindow * window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
