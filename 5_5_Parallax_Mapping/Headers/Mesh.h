#pragma once

#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Texture.h"
#include "Shader.h"

const int max_bone_per_vertex = 4;

using namespace std;

struct VertexLayout
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 normal = { 0.0f, 0.0f, 0.0f };
	glm::vec3 tangent = { 0.0f, 0.0f, 0.0f };
	glm::vec2 texCoords = { 0.0f, 0.0f };
};

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
	enum attribs
	{
		POS_ATTRIB = 0,
		NORMAL_ATTRIB = 1,
		TANGENT_ATTRIB = 2,
		TEXCOORD_ATTRIB = 3,
		BONE_ATTRIB = 4,
		BONE_WEIGHT_ATTRIB = 5,
	};

	vector<VertexLayout> m_layouts;
	vector<unsigned int> m_indices;
	vector<shared_ptr<Texture>> m_textures;
	vector<glm::mat4> m_matrices;

	Material m_material;

	glm::mat4 m_mesh_transform;
	bool m_debug;

protected:
	string m_name;
	unsigned int m_VAO;

public:
	Mesh(string name, vector<VertexLayout> vertices,
		vector<unsigned int> indices = {}, vector<shared_ptr<Texture>> textures = {},
		Material material = {}, vector<glm::mat4> matrices = {});

	virtual void Draw();
	void Draw(Shader& shader);

	void SetupMesh();
	void SetMeshTransform(const mat4& m);
	void SetActive() const;
};

ostream& operator<<(ostream& os, const vec2& v);
ostream& operator<<(ostream& os, const vec3& v);
ostream& operator<<(ostream& os, const vec4& v);
ostream& operator<<(ostream& os, const mat3& m);
ostream& operator<<(ostream& os, const mat4& m);

#endif // !MESH_H
