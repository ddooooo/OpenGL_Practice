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

struct VertexLayout
{
	vec3 position = {0.0f, 0.0f, 0.0f};
	vec3 normal = {0.0f, 0.0f, 0.0f};
	vec3 tangent = { 0.0f, 0.0f, 0.0f };
	vec2 texCoords = {0.0f, 0.0f};
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
	Mesh(vector<VertexLayout> vertices, vector<unsigned int> indices = {}, 
		 vector<unique_ptr<Texture>> textures = {}, Material material = {}, vector<mat4> matrices = {});

	// Render the mesh
	void Draw();
	void Draw(Shader& shader);

	void SetupMesh();
	void SetActive() const;

private:
	vector<VertexLayout> m_vertices;
	vector<unsigned int> m_indices;
	vector<unique_ptr<Texture>> m_textures;
	vector<mat4> m_matrices;
	Material m_material;

	unsigned int m_VAO;

	bool m_debug;
};

ostream& operator<<(ostream& os, const vec2& v);
ostream& operator<<(ostream& os, const vec3& v);
ostream& operator<<(ostream& os, const vec4& v);
ostream& operator<<(ostream& os, const mat3& m);
ostream& operator<<(ostream& os, const mat4& m);
#endif