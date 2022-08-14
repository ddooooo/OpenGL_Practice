#pragma once

#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "Texture.h"

using namespace std;
using namespace glm;

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
	vec3 position;
	vec3 normal = {0.0f, 0.0f, 0.0f};
	vec2 texCoords = {0.0f, 0.0f};
};

struct TextureS
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex> m_vertices;
	vector<unsigned int> m_indices;
	vector<Texture> m_textures;

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices = {}, vector<Texture> textures = {});

	// Render the mesh
	void Draw();
	void Draw(Shader& shader);

	unsigned int GetVAO() { return m_VAO; }

	// Process vertices/indices/textures
	// void ProcessMesh();

	void SetupMesh();

private:
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;


	bool debug = true;
};

#endif
