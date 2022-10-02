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

#include <stb_image.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Config.h"

struct BoneInfo
{
	int id;
	mat4 offset;
	mat4 transformation;
};

typedef unordered_map<string, BoneInfo> BoneInfoMap;

class Model
{
public:

private:	
	vector<unique_ptr<Mesh>> m_meshes;
	vector<shared_ptr<Texture>> m_textures_loaded;

	BoneInfoMap m_bone_info_map;

	mat4 m_root_transformation;

	Assimp::Importer m_importer;
	const aiScene* m_aiScene;

	string m_path;
	string m_directory;
	bool m_gamma_correction;
	bool debug = true;

public:

	Model(const string& path, bool gamma = false);
	void Draw(Shader& shader);
	bool LoadModel();

	// Get functions
	inline const string& GetPath() { return m_path; }
	inline int GetNumBones()
	{
		return static_cast<int>(m_bone_info_map.size());
	}
	BoneInfoMap& GetBoneInfoMap()
	{
		//cout << "GetBoneInfoMap" << endl;
		//for (auto& it : m_bone_info_map)
		//{
		//	cout << "Name: " << it.first << endl;
		//	cout << "id: " << it.second.id << endl;
		//	cout << "offset: ";
		//	cout << it.second.offset << endl;
		//}
		return m_bone_info_map;
	}

	const aiScene* GetScene() { return m_aiScene; }

private:
	// Node and mesh handling functions
	void ProcessNode(aiNode* node, const aiScene* scene);
	unique_ptr<Mesh> ProcessMesh(aiMesh* mesh, const aiScene* scene, const string& name, mat4& m);

	// Bone handling functions
	bool LoadBones(vector<VertexLayout>& vertices, aiMesh* mesh, const aiScene* scene);
	bool LoadSingleBone(vector<VertexLayout>& vertices, shared_ptr<aiBone> bone, int bone_index);
	int GetBoneID(const shared_ptr<aiBone> bone);
	void AddBoneInfo(VertexLayout& vertex, int bone_ID, float weight);

	// Object material handling functions
	Material LoadMaterial(shared_ptr<aiMaterial> mat);

	// Texture handling functions
	vector<shared_ptr<Texture>> LoadMaterialTextures(shared_ptr<aiMaterial> mat, aiTextureType type, string typeName);
	//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

};

ostream& operator<<(ostream& os, aiMatrix4x4 m);
mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& aiMat);
vec3 ConvertVectorToGLMFormat(const aiVector3D& aiVec);
quat ConvertQuatToGLMFormat(const aiQuaternion& aiQuat);

#endif