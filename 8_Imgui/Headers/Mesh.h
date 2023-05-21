#pragma once

#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Buffer.h"

const int max_bone_per_vertex = 4;

using namespace std;

struct Material
{
	glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse = { 0.0f, 0.0f, 0.0f };
	glm::vec3 specular = { 0.0f, 0.0f, 0.0f };
	string type = "";
	float shininess = 0.0f;
};

class Mesh
{
private:
	unique_ptr<VertexBuffer> m_buffers;

	vector<info::VertexLayout> m_layouts;
	vector<unsigned int> m_indices;
public:
	Mesh();

	bool importMeshFromFile(const string& file_path);

	bool loadText(const string& file_path);
	//bool loadFBX(const string& file_path);

	void loadMeshToBuffers();

	void draw();
	//void draw(Shader& shader);

	void bind() const;
	void unbind() const;

};
#endif // !MESH_H
