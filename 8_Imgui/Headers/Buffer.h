#pragma once
#include <SDL.h>
#include <GL/glew.h>
#include "Utils.h"


class VertexBuffer
{
private:
	enum attribs
	{
		POS_ATTRIB = 0,
		NORMAL_ATTRIB = 1,
		TANGENT_ATTRIB = 2,
		TEXCOORD_ATTRIB = 3,
		BONE_ATTRIB = 4,
		BONE_WEIGHT_ATTRIB = 5,
	};

	GLuint m_VAO;
	GLuint m_VBO;
	GLuint m_EBO;

public:
	VertexBuffer();
	void createBuffers(const vector<info::VertexLayout>& layouts, const vector<unsigned int>& indices);
	void bind() const;
	void unbind() const;
};