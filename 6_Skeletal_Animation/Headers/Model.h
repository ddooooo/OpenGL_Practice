#pragma once


#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

using namespace std;
using namespace Assimp;

struct BoneInfo
{
	int id;
	mat4 offset;
};

class Model
{
public:

	Model(const string& path, bool gamma = false);
	void Draw(Shader& shader);
	bool LoadModel();

	int GetNumBones()
	{
		return static_cast<int>(m_bone_infos.size());
	}

private:	
	void ProcessNode(aiNode* node, const aiScene* scene);
	unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const string& name);

	// Bone handling functions
	bool LoadBones(vector<VertexLayout>& vertices, aiMesh* mesh, const aiScene* scene);
	bool LoadSingleBone(vector<VertexLayout>& vertices, aiBone* bone, int bone_index);
	int GetBoneID(const aiBone* bone);
	void AddBoneInfo(VertexLayout& vertex, int bone_ID, float weight);

	// Object material handling functions
	Material LoadMaterial(aiMaterial* mat);
	
	// Texture handling functions
	vector<shared_ptr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

	vector<unique_ptr<Mesh>> m_meshes;
	vector<shared_ptr<Texture>> m_textures_loaded;

	unordered_map<string, BoneInfo> m_bone_infos;

	string m_path;
	string m_directory;
	bool m_gamma_correction;
	int m_bone_counter;

	bool debug = true;
};

ostream& operator<<(ostream& os, aiMatrix4x4 m)
{
	return os << m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << " "
		<< m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << " "
		<< m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << " "
		<< m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4;
}

#endif