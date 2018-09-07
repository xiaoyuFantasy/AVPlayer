// TestOpengl.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "TestOpengl.h"
#include <atlimage.h>
#include <Windows.h>
#include <iostream>
#include "stb\stb_image.h"
//#include <filesystem.h>
#include <shader_m.h>
#include <math.h>
//#include <wingdi.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
using namespace std;

//#include <gl\GL.h>
//#include <gl\GLU.h>

#include <glad/glad.h>
#pragma comment(lib, "glad.lib")

#define INCLUDE_GLFW3 1

#ifdef INCLUDE_GLFW3 
#include <glfw3.h>
#pragma comment(lib, "glfw3.lib")
#endif // INCLUDE_GLFW3 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;


const GLuint WIDTH = 800, HEIGHT = 600;
const int pixel_w = 2160, pixel_h = 1080;
unsigned char buf[pixel_w*pixel_h * 3 / 2] = { 0 };
unsigned char *plane[3];


#define PI 3.1415926

float* verticals;
float* UV_TEX_VERTEX;

int slicesCount = 200;
int parallelsCount = slicesCount / 2;
int indexCount = slicesCount * parallelsCount * 6;
int numPoint = (slicesCount + 1) * (parallelsCount + 1);

short *indices = new short[indexCount];
float *vertexs = new float[numPoint * 3];
float *texcoords = new float[numPoint * 2];


void getPointMatrix()
{
	float const step = (2.0f * PI) / (float)slicesCount;
	float const radius = 1.0f;
	int runCount = 0;
	for (int i = 0; i < parallelsCount + 1; i++) {
		for (int j = 0; j < slicesCount + 1; j++) {
			int vertex = (i * (slicesCount + 1) + j) * 3;
			if (vertexs) {
				vertexs[vertex + 0] = radius * sinf(step * (float)i) * cosf(step * (float)j);
				vertexs[vertex + 1] = radius * cosf(step * (float)i);
				vertexs[vertex + 2] = radius * sinf(step * (float)i) * sinf(step * (float)j);
			}
			if (texcoords) {
				int textureIndex = (i * (slicesCount + 1) + j) * 2;
				texcoords[textureIndex + 0] = (float)j / (float)slicesCount;
				texcoords[textureIndex + 1] = ((float)i / (float)parallelsCount);
			}
			if (indices && i < parallelsCount && j < slicesCount) {
				indices[runCount++] = i * (slicesCount + 1) + j;
				indices[runCount++] = (i + 1) * (slicesCount + 1) + j;
				indices[runCount++] = (i + 1) * (slicesCount + 1) + (j + 1);

				indices[runCount++] = i * (slicesCount + 1) + j;
				indices[runCount++] = (i + 1) * (slicesCount + 1) + (j + 1);
				indices[runCount++] = i * (slicesCount + 1) + (j + 1);
			}
		}
	}
}

#define GLMatrix4DIndentity {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1}
#define GLMatrix4DData {0.999,0.024,-0.007,0,  -0.007,0.540,0.842,0,   0.025,-0.842,0.540,0,   0,0,0.0,1}
#define GLMatrix4DDemoData {1.2,0,0,0,0,1.7,0,0,0,0,-1.0,-1,0,0,-0.2,1}

#ifdef INCLUDE_GLFW3
int CreateGLFWWindows()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the required callback functions
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// build and compile our shader zprogram
	// ------------------------------------
	Shader ourShader("Shader.vsh", "Shader.fsh");
	
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------

	//float vertices[] = {
	//	// positions         // texture coords
	//	1.0f,  0.5f, 0.0f,		//1.0f, 0.0f, // top right
	//	1.0f, -0.5f, 0.0f,		//1.0f, 1.0f, // bottom right
	//	-1.0f, -0.5f, 0.0f,		//0.0f, 1.0f, // bottom left
	//	-1.0f,  0.5f, 0.0f,		//0.0f, 0.0f  // top left 
	//};

	//float texturecoords[] = {
	//	// texture coords
	//	1.0f, 0.0f, // top right
	//	1.0f, 1.0f, // bottom right
	//	0.0f, 1.0f, // bottom left
	//	0.0f, 0.0f  // top left 
	//};

	//unsigned int indices[] = {
	//	0, 1, 3, // first triangle
	//	1, 2, 3  // second triangle
	//};

	//unsigned int indices[] = {  // note that we start from 0!
	//	2,1,0,  // first Triangle
	//	2,3,0   // second Triangle
	//};

	glm::vec3 cubePosition = {
		0.0f,  0.0f,  0.0f
	};

	getPointMatrix();

	unsigned int VBO, VBO2, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &VBO2);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	/*float *vertexs = new float[numPoint * 3];
	float *texcoords = new float[numPoint * 2];
	short *indices = new short[indexCount * 6];*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(GLushort), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, numPoint * 3 * sizeof(GLfloat), vertexs, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, numPoint * 2 * sizeof(GLfloat), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	/*glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texturecoords), texturecoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);*/
	
	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// texture coord attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	
	// load and create a texture 
	// -------------------------
	unsigned int texture[3] = { 0 };
	glGenTextures(3, texture);
	glBindTexture(GL_TEXTURE_2D, texture[0]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture[1]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	 // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texture[2]); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
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
	
	plane[0] = buf;
	plane[1] = plane[0] + pixel_w*pixel_h;
	plane[2] = plane[1] + pixel_w*pixel_h / 4;
	FILE *fileYuv = _fsopen("panor_2160x1080.yuv", "rb", _SH_DENYWR);
	if (!fileYuv)
		return 0;

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Check if any events have been activated (key pressed, mouse moved etc.) and call corresponding response functions
		processInput(window);
		// Render
		// Clear the colorbuffer
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (fread(buf, 1, pixel_w*pixel_h * 3 / 2, fileYuv) != pixel_w*pixel_h * 3 / 2) {
			if (feof(fileYuv))
				fseek(fileYuv, 0, 0);
		}

		// bind Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w, pixel_h, 0, GL_RED, GL_UNSIGNED_BYTE, plane[0]);
		ourShader.setInt("tex_y", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture[1]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[1]);
		ourShader.setInt("tex_u", 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture[2]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, pixel_w / 2, pixel_h / 2, 0, GL_RED, GL_UNSIGNED_BYTE, plane[2]);
		ourShader.setInt("tex_v", 2);

		// activate shader
		ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);
		// camera/view transformation
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		ourShader.setMat4("view", view);
		/*glm::mat4 view(GLMatrix4DData);
		ourShader.setMat4("view", view);*/

		// calculate the model matrix for each object and pass it to shader before drawing
		/*glm::mat4 model;
		model = glm::translate(model, cubePosition);
		float angle = 20.0f * 0;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		ourShader.setMat4("model", model);*/
		glm::mat4 model(GLMatrix4DDemoData);
		ourShader.setMat4("model", model);

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, 0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);

	// Terminates GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}
#endif // INCLUDE_GLFW3

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

int main(int argc, char** argv)
{
    // TODO: 在此放置代码。
	CreateGLFWWindows();
	return 0;
}
