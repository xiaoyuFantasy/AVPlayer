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

//矩阵变换的坐标
float oldx = 0;
float oldy = 0;

//交叉点的坐标
int cx = 0;
int cy = 0;

int angle = 0;

static float rotate = 0;
static int times = 0;

static float centerX = 0.0;
static float centerY = 0.0;
static float centerZ = 0.0;
static bool add = TRUE;

GLFWwindow* window = nullptr;

void display(void)
{
	//printf("oldx=%f, oldy=%f\n", oldx, oldy);
   // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float rotatefs[][4] = {
		{90.0, 1.0, 0.0, 0.0},    //top
		{-90.0, 1.0, 0.0, 0.0},    //bottom
		{90.0, 0.0, 0.0, 1.0},    //front
		{-90.0, 0.0, 1.0, 0.0},    //left
		{-90.0, 0.0, 0.0, 1.0},    //back
		{90.0, 0.0, 1.0, 0.0},    //right
	};

	float translefs[][3] = {
		{0.0, 0.5, 0.0}, //top
		{0.0, -0.5, 0.0},//bottom
		{0.0, 0.0, -0.5},//front
		{-0.5, 0.0, 0.0},//left
		{0.0, 0.0, 0.5}, //back
		{0.5, 0.0, 0.0}     //right
	};

	glPushMatrix(); {
		glColor3f(0, 0, 0);
		glLoadIdentity();  //加载单位矩阵
		glTranslatef(0.0f, 0.0f, -10.0f);
		glRectf(-1.0f, -1.0f, -0.5f, -0.5f);
	}
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  //加载单位矩阵
	glPushMatrix(); 
	{
		glLoadIdentity();  //加载单位矩阵
			//gluLookAt(0, 0, -10.0f, centerX, centerY, centerZ, 0, 1, 0);
		glTranslatef(0.0f, 0.0f, -10.0f);
		glRotatef(angle, 0, 1, 0);
		glScalef(0.5, 0.5, 0.5);
		float rgb[] = { 0.1, 0.3, 1.0 };
		for (int i = 0; i < 6; i++) {
			glPushMatrix(); {
				glTranslatef(translefs[i][0], translefs[i][1], translefs[i][2]);
				glRotatef(rotatefs[i][0], rotatefs[i][1], rotatefs[i][2], rotatefs[i][3]);
				//glScalef(-0.5, -0.5, -0.5);
				glColor3f(rgb[0], rgb[1], rgb[2]); //画笔颜色
				glRectf(-0.5f, -0.5f, 0.5f, 0.5f);

				rgb[0] += 0.2;
				rgb[1] += 0.1;
				rgb[2] -= 0.1;
			}
			glPopMatrix();
		}
	}
	glPopMatrix();

	printf("centerX=%f, centerY=%f\n", centerX, centerY);
	if (add)
	{
		centerX += 0.1;
		centerY += 0.1;
		centerZ += 0.1;
	}
	else {
		centerX -= 0.1;
		centerY -= 0.1;
		centerZ -= 0.1;
	}
	if (centerX > 10)
	{
		add = FALSE;
	}
	else if (centerX < -10) {
		add = TRUE;
	}
	//如果通过鼠标控制，可以把下面两行代码注视掉，留着也没关系
	angle += 1;
	angle %= 360;

	//if (OUTPUT_MODE == 0) {
	//    glFlush();//单缓存GLUT_SINGLE时使用
	//} else {
	//    glutSwapBuffers();//因为使用的是双缓存GLUT_DOUBLE，所以这里必须要交换缓存才会显示
	//}
	//glClear(GL_COLOR_BUFFER_BIT);
	//glFlush();
	glfwSwapBuffers(window);
}

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		//将窗口设置为关闭，跳出循环
		glfwSetWindowShouldClose(window, true);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

	//显示窗口
	glfwMakeContextCurrent(window);

	//设置回调，当窗口大小调整后将调用该回调函数
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad初始化
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("加载失败");
		return -1;
	}

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// 使用循环达到循环渲染效果
	while (!glfwWindowShouldClose(window))
	{
		//自定义输入事件
		//processInput(window);
		//交互缓冲区，否则显示空白
		//glfwSwapBuffers(window);
		//输入输出事件,否则无法对窗口进行交互
		glfwPollEvents();

		/*glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);*/
		//交换缓冲
		display();
	}

	//终止渲染 关闭并清理glfw本地资源
	glfwTerminate();
	return 0;
}

