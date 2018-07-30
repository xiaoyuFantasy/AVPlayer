// TestPanoramic.cpp : �������̨Ӧ�ó������ڵ㡣
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

//����任������
float oldx = 0;
float oldy = 0;

//����������
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
		glLoadIdentity();  //���ص�λ����
		glTranslatef(0.0f, 0.0f, -10.0f);
		glRectf(-1.0f, -1.0f, -0.5f, -0.5f);
	}
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();  //���ص�λ����
	glPushMatrix(); 
	{
		glLoadIdentity();  //���ص�λ����
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
				glColor3f(rgb[0], rgb[1], rgb[2]); //������ɫ
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
	//���ͨ�������ƣ����԰��������д���ע�ӵ�������Ҳû��ϵ
	angle += 1;
	angle %= 360;

	//if (OUTPUT_MODE == 0) {
	//    glFlush();//������GLUT_SINGLEʱʹ��
	//} else {
	//    glutSwapBuffers();//��Ϊʹ�õ���˫����GLUT_DOUBLE�������������Ҫ��������Ż���ʾ
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
		//����������Ϊ�رգ�����ѭ��
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

	//��ʾ����
	glfwMakeContextCurrent(window);

	//���ûص��������ڴ�С�����󽫵��øûص�����
	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad��ʼ��
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("����ʧ��");
		return -1;
	}

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// ʹ��ѭ���ﵽѭ����ȾЧ��
	while (!glfwWindowShouldClose(window))
	{
		//�Զ��������¼�
		//processInput(window);
		//������������������ʾ�հ�
		//glfwSwapBuffers(window);
		//��������¼�,�����޷��Դ��ڽ��н���
		glfwPollEvents();

		/*glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);*/
		//��������
		display();
	}

	//��ֹ��Ⱦ �رղ�����glfw������Դ
	glfwTerminate();
	return 0;
}

