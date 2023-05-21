#include "Buffer.h"

VertexBuffer::VertexBuffer() : m_VAO(0), m_VBO(0), m_EBO(0) {}

void VertexBuffer::createBuffers(const vector<info::VertexLayout>& layouts, const vector<unsigned int>& indices)
{
	cout << "Create vertex buffers " <<  indices.size() << endl;
	cout << "Size of vertex layout: " << layouts.size() << endl;

	// Generate buffers: VAO, VBO, EBO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, layouts.size() * sizeof(info::VertexLayout), &layouts[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


	glEnableVertexAttribArray(POS_ATTRIB);
	glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(info::VertexLayout), (void*)0);

	glEnableVertexAttribArray(NORMAL_ATTRIB);
	glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(info::VertexLayout), (void*)offsetof(info::VertexLayout, normal));

	glEnableVertexAttribArray(TANGENT_ATTRIB);
	glVertexAttribPointer(TANGENT_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(info::VertexLayout), (void*)offsetof(info::VertexLayout, tangent));

	glEnableVertexAttribArray(TEXCOORD_ATTRIB);
	glVertexAttribPointer(TEXCOORD_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(info::VertexLayout), (void*)offsetof(info::VertexLayout, texCoord));

	// Bone 
	//glEnableVertexAttribArray(BONE_ATTRIB);
	//glVertexAttribIPointer(BONE_ATTRIB, 4, GL_INT, sizeof(VertexLayout), (void*)offsetof(VertexLayout, bone_IDs));

	// Bone weights
	//glEnableVertexAttribArray(BONE_WEIGHT_ATTRIB);
	//glVertexAttribPointer(BONE_WEIGHT_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, weights));

	// Reset the vertex array binder
	glBindVertexArray(0);
}

void VertexBuffer::bind() const
{
	glBindVertexArray(m_VAO);
}

void VertexBuffer::unbind() const
{
	glBindVertexArray(0);
}

