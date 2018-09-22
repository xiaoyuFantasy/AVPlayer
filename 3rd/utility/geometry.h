#pragma once

#ifdef GLAD
#include <glad/glad.h>
#endif // GLAD
#ifdef GLEW
#include <GL\glew.h>
#endif // GLEW
#include <vector>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>

void generateSphereGeometry(GLfloat radius, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& texcoords)
{
	// ���ȷ����и�100��
	const GLuint VERTICAL_SLICE = 200;
	GLfloat vertical_step = (GLfloat)(glm::two_pi<GLfloat>() / VERTICAL_SLICE);
	// γ�ȷ����и�50��
	const GLuint HORIZONTAL_SLICE = 100;
	GLfloat horizontal_step = (GLfloat)(glm::pi<GLfloat>() / HORIZONTAL_SLICE);

	GLuint start_index = 0;
	GLuint current_index = 0;
	// γ�ȷ����Ͻ�����ָ��50�Σ����и��50����ͬ�뾶��ͬ��Բ
	for (size_t i = 0; i <= HORIZONTAL_SLICE; ++i)
	{
		start_index = current_index;
		GLfloat vertical_angle = horizontal_step * i;
		GLfloat z_coord = radius * std::cos(vertical_angle);
		GLfloat sub_radius = radius * std::sin(vertical_angle);
		// ���ȷ��������и��100��
		for (size_t j = 0; j <= VERTICAL_SLICE; j++)
		{
			GLfloat horizontal_angle = vertical_step * j;
			GLfloat x_coord = sub_radius * std::cos(horizontal_angle);
			GLfloat y_coord = sub_radius * std::sin(horizontal_angle);

			// һȦ�����ˣ������ص��غ�
			if (j == VERTICAL_SLICE)
			{
				vertices.push_back(vertices[start_index]);
				vertices.push_back(vertices[start_index + 1]);
				vertices.push_back(vertices[start_index + 2]);
			}
			else
			{
				// �����ڼ���x, y, z����ʱ����ϵ+Z�����ϣ�+X��ָ����Ļ��
				// ���������Ҫ��������ϵΪ��������ϵ
				vertices.push_back(x_coord);
				vertices.push_back(z_coord);
				vertices.push_back(y_coord);
			}
			// ����ÿһȦ����ʼ���������������ÿһȦ���غϵ�
			current_index += 3;

			if (i > 0 && j > 0)
			{
				// ������һȦ�еĶ�������
				GLuint bottom_ring_a = (VERTICAL_SLICE + 1)*i + j;
				GLuint bottom_ring_b = (VERTICAL_SLICE + 1)*i + j - 1;
				// ������һȦ�еĶ�������
				GLuint top_ring_a = (VERTICAL_SLICE + 1)*(i - 1) + j;
				GLuint top_ring_b = (VERTICAL_SLICE + 1)*(i - 1) + j - 1;

				// ��ʱ�ӷ�����������α������޳������
				indices.push_back(bottom_ring_a);
				indices.push_back(top_ring_a);
				indices.push_back(top_ring_b);
				indices.push_back(bottom_ring_a);
				indices.push_back(top_ring_b);
				indices.push_back(bottom_ring_b);

			}

			texcoords.push_back((GLfloat)j / VERTICAL_SLICE);
			texcoords.push_back((GLfloat)i / HORIZONTAL_SLICE);
		}
	}
}

void generatePlaneGeometry(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& texcoords)
{
	GLfloat vert[] = {
		1.0f,  1.0f, 0.0f,    // ����
		1.0f,  -1.0f, 0.0f,   // ����
		-1.0f, -1.0f, 0.0f,    // ����
		-1.0f,  1.0f, 0.0f,    // ����
	};
	for (int i = 0; i < 12; i++)
		vertices.push_back(vert[i]);

	GLfloat texs[] = {
		1.0f, 0.0f,	// top right
		1.0f, 1.0f,	// bottom right
		0.0f, 1.0f,	// bottom left
		0.0f, 0.0f	// top left 
	};
	for (int i = 0; i < 8; ++i)
		texcoords.push_back(texs[i]);

	GLuint inds[] = {
		0, 3, 2,
		0, 2, 1
	};
	for (int i = 0; i < 6; i++)
		indices.push_back(inds[i]);
}