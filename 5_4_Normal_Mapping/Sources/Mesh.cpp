#include <iostream>

#include "Mesh.h"

using namespace std;

Mesh::Mesh(vector<VertexLayout> vertices, vector<unsigned int> indices, 
		   vector<unique_ptr<Texture>> textures, Material material, vector<mat4> matrices)
	: m_vertices(vertices), m_indices(indices), m_matrices(matrices),
	  m_textures(move(textures)), m_material(material), m_VAO(0), m_debug(true)
{

	for (auto& it : m_vertices)
	{
		cout << it.position.x << " " << it.position.y << " " << it.position.z << endl;
	}

}

void Mesh::Draw()
{
	if (m_debug)
	{
		//cout << "Draw Primitive!" << endl;
		//cout << m_indices.size() << endl;
		m_debug = false;
	}

	glBindVertexArray(m_VAO);
	// Already stored indices array into element(index) buffer object in setupmesh function,
	// so the last argument should be nullptrt
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);

	//glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader)
{
	unsigned int diffuse_index = 1;
	unsigned int specular_index = 1;
	unsigned int normal_index = 1;
	unsigned int height_index = 1;
	
	if (m_debug)
	{
		cout << "Draw mesh! with " << m_indices.size() << " and " << m_vertices.size() << " and " << m_textures.size() << endl;
	}

	//glUniform1i(glGetUniformLocation(shader.GetShaderProgram))

	shader.SetActive();

	if (m_debug)
	{
		//cout << "Set mat.ambient " << m_material.ambient.x <<  endl;
	}

	shader.SetVec3("mat.ambient", m_material.ambient);

	if (m_debug)
	{
		//cout << "Set mat.diffuse " << m_material.diffuse.x << endl;
	}

	shader.SetVec3("mat.diffuse", m_material.diffuse);

	if (m_debug)
	{
		//cout << "Set mat.specular " << m_material.specular.x << endl;
		//debug = false;
	}

	shader.SetVec3("mat.specular", m_material.specular);
	shader.SetFloat("mat.shininess", m_material.shininess);
	// Set texture before draw a mesh
	for (int i = 0; i < m_textures.size(); ++i)
	{
		//cout << "hi" << endl;
		glActiveTexture(GL_TEXTURE0 + i);

		string index;
		string type = m_textures[i]->GetType();
		
		if (type == "texture_diffuse")
			index = to_string(diffuse_index++);
		else if (type == "texture_specular")
			index = to_string(specular_index++);
		else if (type == "texture_normal")
			index = to_string(normal_index++);
		else if (type == "texture_height")
			index = to_string(height_index);
		
		//m_textures[i].SetActive();
		if (m_debug)
		{
			cout << "texture." + type + index << endl;
			cout << "Texture from " << m_textures[i]->GetPath() << endl;
		}
		//glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), ("tex." + type + index).c_str()), i);
		
		shader.SetInt(("tex." + type + index).c_str(), i);
		m_textures[i]->SetActive();
		//glBindTexture(GL_TEXTURE_2D, m_textures[i]->GetTextureID());
	}
	
	glBindVertexArray(m_VAO);
	if (m_matrices.size() <= 1)
	{
		// Draw mesh(triangle) with vertices
		if (m_debug)
		{
			cout << "Ready to draw meshes " << m_matrices.size() << endl;
			m_debug = false;
		}
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		if (m_debug)
		{
			//cout << "Ready to draw meshes " << m_matrices.size() << endl;
			m_debug = false;
		}
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), 
			             GL_UNSIGNED_INT, 0, static_cast<unsigned int>(m_matrices.size()));
	}
	
	// Set back to default vertex array
	glBindVertexArray(0);

	// Set everything back to default texture
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	cout << "SetupMesh" << endl;
	// Create vertex array objects, vertex buffering object, Element buffering object
	
	unsigned int VBO;
	unsigned int EBO;
	unsigned int IBO;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexLayout), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	
	cout << "Size: " <<  sizeof(VertexLayout) << endl;

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
	
	if (m_matrices.size() > 1)
	{
		cout << "Setup instance mesh" << endl;
		glGenBuffers(1, &IBO);
		glBindBuffer(GL_ARRAY_BUFFER, IBO);
		glBufferData(GL_ARRAY_BUFFER, m_matrices.size() * sizeof(mat4), &m_matrices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)0);
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(1 * sizeof(vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(2 * sizeof(vec4)));
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(3 * sizeof(vec4)));

		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
	}

	glBindVertexArray(0);
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

ostream& operator<<(ostream& os, const mat4& m)
{
	return os << "\n" << m[0] << "\n" << m[1] << "\n" << m[2] << "\n" << m[3];
}



