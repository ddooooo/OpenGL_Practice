#pragma once


#ifndef MODEL_H
#define MODEL_H

#include <iostream>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

using namespace std;
using namespace Assimp;

class Model
{
public:

	Model(const string& path, bool gamma = false);
	void Draw(Shader& shader);
	bool LoadModel();
private:	
	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	
	// Object material handling functions
	Material LoadMaterial(aiMaterial* mat);
	
	// Texture handling functions
	vector<TextureS> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);
	
	vector<Vertex> m_vertices;
	vector<unsigned int> m_indices;
	vector<TextureS> m_textures;
	Material m_material;

	vector<Mesh> m_meshes;
	vector<TextureS> m_textures_loaded;
	vector<Material> m_materials_loaded;

	string m_path;
	string m_directory;
	bool m_gamma_correction;
	bool debug = true;
};

#endif