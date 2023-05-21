//#include "Model.h"
//
//#include <stb/stb_image.h>
//
//#include <fstream>
//#include <sstream>
//
//Model::Model(string const& path, bool gamma) : m_path(path), m_gamma_correction(gamma)
//{}
//
//void Model::Draw(Shader& shader)
//{
//	if (debug)
//	{
//		//cout << "The size of mesh is " << m_meshes.size() << endl;
//		debug = false;
//	}
//
//	for (unsigned int i = 0; i < m_meshes.size(); ++i)
//		m_meshes[i]->Draw(shader);
//}
//
//bool Model::LoadModel()
//{
//	cout << "Load model! " << m_path <<  endl;
//
//	Importer importer;
//	const aiScene* scene = importer.ReadFile(m_path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
//												   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
//
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//	{
//		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
//		return false;
//	}
//
//	// The directory path of the file path
//	m_directory = m_path.substr(0, m_path.find_last_of('/'));
//	cout << "directory: " << m_directory << endl;
//	// Process ASSIMP's root node recursively
//	ProcessNode(scene->mRootNode, scene);
//
//	return true;
//}
//
//void Model::ProcessNode(aiNode* node, const aiScene* scene)
//{
//	//cout << "Process Node!" << endl;
//	// Loop through all mesh indices in the node and process the actual meshes in the scene object
//	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
//	{
//		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
//		unique_ptr<Mesh> mesh = ProcessMesh(ai_mesh, scene);
//		mesh->SetupMesh();
//		m_meshes.push_back(move(mesh));
//	}
//
//	// Process all the children node recursively for processing a mesh of each children node
//	for (unsigned int i = 0; i < node->mNumChildren; ++i)
//	{
//		ProcessNode(node->mChildren[i], scene);
//	}
//}
//
//unique_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
//{
//	//cout << "Process Mesh!" << endl;
//
//	// Rest the vertices and indices
//
//	m_vertices.clear();
//	m_indices.clear();
//	m_textures.clear();
//
//	// Data for mesh
//
//	unsigned int num_faces = mesh->mNumFaces;
//	unsigned int num_vertices = mesh->mNumVertices;
//	unsigned int num_indices = num_faces * 3;
//	
//	for (unsigned int i = 0; i < num_vertices; ++i)
//	{
//		VertexLayout vertex;
//		vec3 temp;
//		
//		// Convert Assimp::vec3 to glm::vec3
//		temp.x = mesh->mVertices[i].x;
//		temp.y = mesh->mVertices[i].y;
//		temp.z = mesh->mVertices[i].z;
//		// Set position of each vertex
//		vertex.position = temp;
//
//		//  Set normal of each vertex
//		if (mesh->HasNormals())
//		{
//			temp.x = mesh->mNormals[i].x;
//			temp.y = mesh->mNormals[i].y;
//			temp.z = mesh->mNormals[i].z;
//			vertex.normal = temp;
//		}
//
//		// set texture coordinate of each vertex
//		if (mesh->mTextureCoords[0])
//		{
//			vec2 texCoords_temp;
//
//			texCoords_temp.x = mesh->mTextureCoords[0][i].x;
//			texCoords_temp.y = mesh->mTextureCoords[0][i].y;
//			vertex.texCoords = texCoords_temp;
//		}
//		else
//			vertex.texCoords = vec2(0.0f, 0.0f);
//		
//		m_vertices.push_back(vertex);
//	}
//	
//	// Get indices of the mesh faces(triangle)
//	for (unsigned int i = 0; i < num_faces; ++i)
//	{
//		const aiFace& face = mesh->mFaces[i];
//		//cout << "Face" << i + 1 << " has " << face.mIndices[0] << " " << face.mIndices[1] << " " << face.mIndices[2] << endl;
// 		m_indices.push_back(face.mIndices[0]);
//		m_indices.push_back(face.mIndices[1]);
//		m_indices.push_back(face.mIndices[2]);
//	}
//
//	// Load materials
//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//	
//	m_material = LoadMaterial(material);
//
//	// Load Diffuse from texture
//	vector<unique_ptr<Texture>> diffuseMaps = move(LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"));
//	m_textures.insert(m_textures.end(), 
//				make_move_iterator(diffuseMaps.begin()), 
//				make_move_iterator(diffuseMaps.end())
//	);
//
//	// Load specular from texture
//	vector<unique_ptr<Texture>> specularMaps = move(LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"));
//	m_textures.insert(m_textures.end(), 
//				make_move_iterator(specularMaps.begin()), 
//				make_move_iterator(specularMaps.end())
//	);
//	
//	// Load normal map from texture
//	vector<unique_ptr<Texture>> normalMaps = move(LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal"));
//	m_textures.insert(m_textures.end(), 
//				make_move_iterator(normalMaps.begin()), 
//				make_move_iterator(normalMaps.end())
//	);
//	
//	// Load height map from texture
//	vector<unique_ptr<Texture>> heightMaps = move(LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient"));
//	m_textures.insert(m_textures.end(), 
//				make_move_iterator(heightMaps.begin()), 
//				make_move_iterator(heightMaps.end())
//	);
//
//	return make_unique<Mesh>(m_vertices, m_indices, move(m_textures), m_material);
//}
//
//Material Model::LoadMaterial(aiMaterial* mat)
//{
//	aiString str;
//	Material material;
//
//	if (AI_SUCCESS != mat->Get(AI_MATKEY_NAME, str))
//	{
//		// if material does not have any name
//		// just return empty vector...
//		cerr << "Material does not have any name..." << endl;
//		return material;
//	}
//
//	aiColor3D color(0.0f, 0.0f, 0.0f);
//
//	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
//	{
//		material.ambient = { color.r, color.g, color.b };
//		//cout << "Set ambient " << material.ambient.x << endl;
//	}
//	else
//	{
//		material.ambient = { 0.0f, 0.0f, 0.0f };
//	}
//
//	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
//	{
//		material.diffuse = { color.r, color.g, color.b };
//		//cout << "Set Diffuse" << material.diffuse.x << endl;
//	}
//	else
//	{
//		material.diffuse = { 0.0f, 0.0f, 0.0f };
//	}
//
//	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
//	{
//		material.specular = { color.r, color.g, color.b };
//		//cout << "Set specular" << material.specular.x << endl;
//	}
//	else
//	{
//		material.specular = { 0.0f, 0.0f, 0.0f };
//	}
//
//	float metal_factor = 0.0;
//
//	if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, metal_factor))
//	{
//		material.shininess = metal_factor;
//		//cout << "Metallic Factor is " << material.shininess << endl;
//		
//	}
//
//	return material;
//}
//
//vector<unique_ptr<Texture>> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
//{
//	//cout << "Load Material Texture " << typeName << " " << mat->GetTextureCount(type) << endl;
//
//	vector<unique_ptr<Texture>> textures;
//	
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		bool skip = false;
//		string path = m_directory + '/' + str.C_Str();
//		cout << "path: " << path << endl;
//
//		// Check if the texture was loaded before
//		for (unsigned int j = 0; j < m_textures_loaded.size(); ++j)
//		{
//			if (strcmp(m_textures_loaded[j]->GetPath().data(), path.c_str()) == 0)
//			{
//				// If the texture was loaded before, 
//				// just push the loaded texture to the textures container
//				textures.push_back(move(m_textures_loaded[j]));
//				skip = true;
//				break;
//			}
//		}
//
//		if (!skip)
//		{
//			unique_ptr<Texture> texture = make_unique<Texture>(path, typeName);
//			texture->LoadTexture();
//			textures.push_back(move(texture));
//			m_textures_loaded.push_back(move(texture));
//		}
//	}
//	
//	return textures;
//}