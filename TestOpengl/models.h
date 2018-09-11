#pragma once

#include <glad/glad.h>
#include <vector>
#include <cmath>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/constants.hpp>
#include <gtc/type_ptr.hpp>

void generateSphereGeometry3(GLfloat radius, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& texcoords)
{
	float x = 0;
	float y = 0;
	float z = 0;

	int index = 0;
	int index1 = 0;
	float r = radius;//����뾶
	double d = glm::pi<GLfloat>() / 180;//ÿ�ε����Ļ���
	for (int i = 0; i < 180; i += 1) {
		double d1 = i * glm::pi<GLfloat>() / 180;
		for (int j = 0; j < 360; j += 1) {
			//����������зֵĳ�СƬ���εĶ������꣨������������ɣ����������㶥�㣩
			double d2 = j * glm::pi<GLfloat>() / 180;

			/*verticals[index++] = (float)(x + r * sin(d1 + d) * cos(d2 + d));
			verticals[index++] = (float)(y + r * cos(d1 + d));
			verticals[index++] = (float)(z + r * sin(d1 + d) * sin(d2 + d));*/
			vertices.push_back((float)(x + r * sin(d1 + d) * cos(d2 + d)));
			vertices.push_back((float)(y + r * cos(d1 + d)));
			vertices.push_back((float)(z + r * sin(d1 + d) * sin(d2 + d)));

			//����������зֵĳ�СƬ�����ε���������
			/*UV_TEX_VERTEX[index1++] = (j + cap_W) * 1.0f / 360;
			UV_TEX_VERTEX[index1++] = (i + cap_H) * 1.0f / 180;*/
			texcoords.push_back((j + 1) * 1.0f / 360);
			texcoords.push_back((j + 1) * 1.0f / 180);

			vertices.push_back((float)(x + r * sin(d1) * cos(d2)));
			vertices.push_back((float)(y + r * cos(d1)));
			vertices.push_back((float)(z + r * sin(d1) * sin(d2)));

			texcoords.push_back(j * 1.0f / 360);
			texcoords.push_back(i * 1.0f / 180);

			vertices.push_back((float)(x + r * sin(d1) * cos(d2 + d)));
			vertices.push_back((float)(y + r * cos(d1)));
			vertices.push_back((float)(z + r * sin(d1) * sin(d2 + d)));

			texcoords.push_back((j + 1) * 1.0f / 360);
			texcoords.push_back(i * 1.0f / 180);

			vertices.push_back((float)(x + r * sin(d1 + d) * cos(d2 + d)));
			vertices.push_back((float)(y + r * cos(d1 + d)));
			vertices.push_back((float)(z + r * sin(d1 + d) * sin(d2 + d)));

			texcoords.push_back((j + 1) * 1.0f / 360);
			texcoords.push_back((i + 1) * 1.0f / 180);

			vertices.push_back((float)(x + r * sin(d1 + d) * cos(d2)));
			vertices.push_back((float)(y + r * cos(d1 + d)));
			vertices.push_back((float)(z + r * sin(d1 + d) * sin(d2)));

			texcoords.push_back(j * 1.0f / 360);
			texcoords.push_back((i + 1) * 1.0f / 180);

			vertices.push_back((float)(x + r * sin(d1) * cos(d2)));
			vertices.push_back((float)(y + r * cos(d1)));
			vertices.push_back((float)(z + r * sin(d1) * sin(d2)));

			texcoords.push_back(j * 1.0f / 360);
			texcoords.push_back(i * 1.0f / 180);
		}
	}
}

void generateSphereGeometry2(std::vector<GLfloat>& vertices, std::vector<GLuint>& indices, std::vector<GLfloat>& texcoords)
{
	int slicesCount = 200;
	int parallelsCount = slicesCount / 2;
	int indexCount = slicesCount * parallelsCount * 6;
	int vertexCount = (slicesCount + 1) * (parallelsCount + 1);
	float const step = (2.0f * glm::pi<GLfloat>()) / (float)slicesCount;
	float const radius = 2.0f;

	for (int i = 0; i < parallelsCount + 1; i++) {
		for (int j = 0; j < slicesCount + 1; j++) {
			int vertex = (i * (slicesCount + 1) + j) * 3;
			/*if (vertexBufferData) {
				vertexBufferData[vertex + 0] = radius * sinf(step * (float)i) * cosf(step * (float)j);
				vertexBufferData[vertex + 1] = radius * cosf(step * (float)i);
				vertexBufferData[vertex + 2] = radius * sinf(step * (float)i) * sinf(step * (float)j);
			}*/
			vertices.push_back(radius * sinf(step * (float)i) * cosf(step * (float)j));
			vertices.push_back(radius * cosf(step * (float)i));
			vertices.push_back(radius * sinf(step * (float)i) * sinf(step * (float)j));

			/*if (textureBufferData) {
				
				textureBufferData[textureIndex + 0] = (float)j / (float)slicesCount;
				textureBufferData[textureIndex + 1] = ((float)i / (float)parallelsCount);
			}*/
			texcoords.push_back((float)j / (float)slicesCount);
			texcoords.push_back(((float)i / (float)parallelsCount));

			/*if (indexBufferData && i < parallelsCount && j < slicesCount) {
				indexBufferData[runCount++] = i * (slicesCount + 1) + j;
				indexBufferData[runCount++] = (i + 1) * (slicesCount + 1) + j;
				indexBufferData[runCount++] = (i + 1) * (slicesCount + 1) + (j + 1);

				indexBufferData[runCount++] = i * (slicesCount + 1) + j;
				indexBufferData[runCount++] = (i + 1) * (slicesCount + 1) + (j + 1);
				indexBufferData[runCount++] = i * (slicesCount + 1) + (j + 1);
			}*/

			if (i < parallelsCount && j < slicesCount) {
				indices.push_back(i * (slicesCount + 1) + j);
				indices.push_back((i + 1) * (slicesCount + 1) + j);
				indices.push_back((i + 1) * (slicesCount + 1) + (j + 1));

				indices.push_back(i * (slicesCount + 1) + j);
				indices.push_back((i + 1) * (slicesCount + 1) + (j + 1));
				indices.push_back(i * (slicesCount + 1) + (j + 1));
			}
			
		}
	}
}

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
			texcoords.push_back(1 - (GLfloat)i / HORIZONTAL_SLICE);
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