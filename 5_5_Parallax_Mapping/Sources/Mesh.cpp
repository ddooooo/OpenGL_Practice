#include "Mesh.h"

using namespace std;

Mesh::Mesh(string name, vector<VertexLayout> layouts, vector<unsigned int> indices, vector<shared_ptr<Texture>> textures, Material material, vector<mat4> matrices)
	: m_name(name), m_layouts(layouts), m_indices(indices), m_matrices(matrices), m_textures(textures), m_material(material),
	m_VAO(0), m_mesh_transform(1.0f), m_debug(true) {}

void Mesh::Draw()
{
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
}

void Mesh::SetupMesh()
{
	unsigned int VBO;
	unsigned int EBO;

	// Generate buffers: VAO, VBO, EBO
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_layouts), &m_layouts[0], GL_STATIC_DRAW);
	//m_vertices.size() * sizeof(VertexLayout)
	// Load a data into element buffers object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)0);
	// Normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, normal));
	// Tangent
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, tangent));
	// Texture Coordinate
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, texCoords));


	// Bone 
	//glEnableVertexAttribArray(BONE_ATTRIB);
	//glVertexAttribIPointer(BONE_ATTRIB, 4, GL_INT, sizeof(VertexLayout), (void*)offsetof(VertexLayout, bone_IDs));

	// Bone weights
	//glEnableVertexAttribArray(BONE_WEIGHT_ATTRIB);
	//glVertexAttribPointer(BONE_WEIGHT_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, weights));

	// Reset the vertex array binder
	glBindVertexArray(0);
}

void Mesh::SetMeshTransform(const mat4& m)
{
	m_mesh_transform = m;
}

void Mesh::SetActive() const
{
	glBindVertexArray(m_VAO);
}

ostream& operator<<(ostream& os, const vec2& v)
{
	return os << v.x << " " << v.y;
}

ostream& operator<<(ostream& os, const vec3& v)
{
	return os << v.x << " " << v.y << " " << v.z;
}

ostream& operator<<(ostream& os, const vec4& v)
{
	return os << v.x << " " << v.y << " " << v.z << " " << v.w;
}

ostream& operator<<(ostream& os, const mat3& m)
{
	return os << "\n" << m[0] << "\n" << m[1] << "\n" << m[2];
}

//ostream& operator<<(ostream& os, const mat4& m)
//{
//	return os << fixed << setprecision(1) << "\n"
//		<< m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n"
//		<< m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n"
//		<< m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n"
//		<< m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3];
//}
