#include "Mesh.h"
#include <iostream>
using namespace std;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<TextureS> textures, Material material)
{
	//cout << "Mesh Object Created" << endl;

	m_vertices = vertices;
	m_indices = indices;
	m_textures = textures;

	m_material = material;
}

//void Mesh::ProcessMesh()
//{
//	for(int i=0; i<vertices.size())
//}

void Mesh::Draw()
{
	if (debug)
	{
		cout << "Draw mesh! with vertices " << endl;
		//cout << m_indices.size() << endl;
		debug = false;
	}

	glBindVertexArray(m_VAO);
	// Already stored indices array into element(index) buffer object in setupmesh function, s
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
	
	if (debug)
	{
		cout << "Draw mesh! with " << m_indices.size() << " and " << m_vertices.size() << " and " << m_textures.size() << endl;
		
	}

	//glUniform1i(glGetUniformLocation(shader.GetShaderProgram))

	shader.SetActive();

	if (debug)
	{
		//cout << "Set mat.ambient " << m_material.ambient.x <<  endl;
	}

	shader.SetVec3("mat.ambient", m_material.ambient);

	if (debug)
	{
		//cout << "Set mat.diffuse " << m_material.diffuse.x << endl;
	}

	shader.SetVec3("mat.diffuse", m_material.diffuse);

	if (debug)
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
		string type = m_textures[i].type;
		
		if (type == "texture_diffuse")
			index = to_string(diffuse_index++);
		else if (type == "texture_specular")
			index = to_string(specular_index++);
		else if (type == "texture_normal")
			index = to_string(normal_index++);
		else if (type == "texture_height")
			index = to_string(height_index);
		
		//shader.SetInt( (type + index).c_str(), i);
		//m_textures[i].SetActive();
		if (debug)
		{
			cout << "texture." + type + index << endl;
			cout << "Texture from " << m_textures[i].path << endl;
		}
		glUniform1i(glGetUniformLocation(shader.GetShaderProgram(), ("tex." + type + index).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	
	if (debug)
	{
		cout << "Ready to draw mesh" << endl;
		debug = false;

	}

	// Draw mesh(triangle) with vertices
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
	
	// Set back to default vertex array
	glBindVertexArray(0);

	// Set everything back to default texture
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::SetupMesh()
{
	//cout << "Set up Mesh in mesh class" << endl;
	
	/*for (int i = 0; i < m_vertices.size(); ++i)
	{
		cout << i + 1 << "th: " << m_vertices[i].normal.x << " " << m_vertices[i].normal.y << " "
			<< m_vertices[i].normal.z << endl;
	}*/

	/*for (int i = 0; i < m_indices.size(); ++i)
	{
		cout << i + 1 << "th: " << m_indices[i] << endl;
	}*/

	// Create vertex array objects, vertex buffering object, Element buffering object
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	// Load a data into element buffers object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	
}