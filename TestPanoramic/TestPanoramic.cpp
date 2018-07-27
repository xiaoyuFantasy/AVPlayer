// TestPanoramic.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <glad\glad.h>
#pragma comment(lib, "glad.lib")

#include <glfw3.h>
#pragma comment(lib, "glfw3.lib")

#include <gl\GL.h>
#pragma comment(lib, "opengl32.lib")

extern "C" {
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libswresample\swresample.h"
#include "libavdevice\avdevice.h"
#include "libavfilter\avfilter.h"
#include "libavutil\hwcontext.h"
#include "libavutil\imgutils.h"
#include "libavutil\time.h"
}

int main()
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

    return 0;
}

