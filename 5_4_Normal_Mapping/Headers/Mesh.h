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
	vec3 position = {0.0f, 0.0f, 0.0f};
	vec3 normal = {0.0f, 0.0f, 0.0f};
	vec2 texCoords = {0.0f, 0.0f};
};

struct TextureS
{
	unsigned int id;
	string type;
	string path;
};

struct Material
{
	vec3 ambient = {0.0f, 0.0f, 0.0f};
	vec3 diffuse = { 0.0f, 0.0f, 0.0f };
	vec3 specular = { 0.0f, 0.0f, 0.0f };
	string type = "";
	float shininess = 0.0f;
};

class Mesh
{
public:
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices = {}, 
		vector<TextureS> textures = {}, vector<mat4> matrices = {}, Material material = {});

	// Render the mesh
	void Draw();
	void Draw(Shader& shader);

	unsigned int GetVAO() const { return m_VAO; }
	void SetupMesh();
	void SetActive() const;

	// Process vertices/indices/textures
	// void ProcessMesh();

private:
	vector<Vertex> m_vertices;
	vector<unsigned int> m_indices;
	vector<TextureS> m_textures;
	vector<mat4> m_matrices;
	Material m_material;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_EBO;
	unsigned int m_IBO;

	bool m_debug;
};
#endif