#include <iomanip> 

#include "Mesh.h"

using namespace std;

Mesh::Mesh(string name, vector<VertexLayout> vertices, 
		   vector<unsigned int> indices, vector<shared_ptr<Texture>> textures, 
		   Material material, vector<mat4> matrices)
	: m_name(name), m_vertices(vertices), m_indices(indices), m_matrices(matrices),
	  m_textures(textures), m_material(material),
	  m_VAO(0), m_mesh_transform(1.0f), m_debug(true)
{}

void Mesh::Draw()
{
	if (m_debug)
	{
		cout << "Draw Primitive!" << endl;
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
	m_debug = false;
	shader.SetActive();
	
	if (m_debug)
	{
		//cout << "Draw mesh: " <<  m_name << ": " << m_indices.size() << " and " << m_vertices.size() << " and " << m_textures.size() << endl;
		//cout << "Transformation mesh: " << endl;
		//cout << m_mesh_transform << endl;
	}
	shader.SetMat4("adjust", m_mesh_transform);

	//glUniform1i(glGetUniformLocation(shader.GetShaderProgram))


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
			cout << "tex." + type + index << endl;
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
			//cout << "Ready to draw meshes " << endl;
			m_debug = false;
		}
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		if (m_debug)
		{
			cout << "Ready to draw meshes " << m_matrices.size() << endl;
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
	//cout << "Setup Mesh: " << m_vertices.size() << endl;
	//// Create vertex array objects, vertex buffering object, Element buffering object
	//cout << "Bone ids " << endl;
	//cout << m_vertices[0].bone_IDs[0] << " " << m_vertices[0].bone_IDs[1] << " " << m_vertices[0].bone_IDs[2] << " " << m_vertices[0].bone_IDs[3] << endl;
	//cout << (m_vertices[0].bone_IDs) << endl;
	//cout << endl;
	unsigned int VBO;
	unsigned int EBO;
	//unsigned int IBO;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexLayout), &m_vertices[0], GL_STATIC_DRAW);

	// Load a data into element buffers object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	
	// Position
	glEnableVertexAttribArray(POS_ATTRIB);
	glVertexAttribPointer(POS_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)0);
	// Normal
	glEnableVertexAttribArray(NORMAL_ATTRIB);
	glVertexAttribPointer(NORMAL_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, normal));
	// Tangent
	glEnableVertexAttribArray(TANGENT_ATTRIB);
	glVertexAttribPointer(TANGENT_ATTRIB, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, tangent));
	// Texture Coordinate
	glEnableVertexAttribArray(TEXCOORD_ATTRIB);
	glVertexAttribPointer(TEXCOORD_ATTRIB, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, texCoords));
	
	glEnableVertexAttribArray(BONE_ATTRIB);
	glVertexAttribIPointer(BONE_ATTRIB, 4, GL_INT,  sizeof(VertexLayout), (void*)offsetof(VertexLayout, bone_IDs));

	glEnableVertexAttribArray(BONE_WEIGHT_ATTRIB);
	glVertexAttribPointer(BONE_WEIGHT_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, weights));
	
	
	glEnableVertexAttribArray(BONE_WEIGHT_ATTRIB);
	glVertexAttribPointer(BONE_WEIGHT_ATTRIB, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLayout), (void*)offsetof(VertexLayout, weights));
	
	/*if (m_matrices.size() > 1)
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
	}*/

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

ostream& operator<<(ostream& os, const mat4& m)
{
	return os << fixed << setprecision(1) << "\n" 
		      << m[0][0] << " " << m[1][0] << " " << m[2][0] << " " << m[3][0] << "\n"
			  << m[0][1] << " " << m[1][1] << " " << m[2][1] << " " << m[3][1] << "\n"
			  << m[0][2] << " " << m[1][2] << " " << m[2][2] << " " << m[3][2] << "\n"
			  << m[0][3] << " " << m[1][3] << " " << m[2][3] << " " << m[3][3];
}



