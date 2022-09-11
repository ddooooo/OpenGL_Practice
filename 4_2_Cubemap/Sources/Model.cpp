#include "Model.h"

#include <stb_image.h>

#include <fstream>
#include <sstream>

Model::Model(bool gamma) : m_gamma_correction(gamma)
{

}

void Model::Draw(Shader& shader)
{
	if (debug)
	{
		//cout << "The size of mesh is " << m_meshes.size() << endl;
		debug = false;
	}

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
		m_meshes[i].Draw(shader);
}

bool Model::LoadModel(string const & path)
{
	//cout << "Load model! " << path <<  endl;

	Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
												   aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return false;
	}

	// The directory path of the file path
	m_directory = path.substr(0, path.find_last_of('/'));
	
	// Process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene);

	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	//cout << "Process Node!" << endl;
	// Loop through all mesh indices in the node and process the actual meshes in the scene object
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh mesh = ProcessMesh(ai_mesh, scene);
		mesh.SetupMesh();
		m_meshes.push_back(mesh);
	}

	// Process all the children node recursively for processing a mesh of each children node
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	//cout << "Process Mesh!" << endl;

	// Rest the vertices and indices

	m_vertices.clear();
	m_indices.clear();
	m_textures.clear();

	// Data for mesh

	unsigned int num_faces = mesh->mNumFaces;
	unsigned int num_vertices = mesh->mNumVertices;
	unsigned int num_indices = num_faces * 3;
	
	for (unsigned int i = 0; i < num_vertices; ++i)
	{
		Vertex vertex;
		vec3 temp;
		
		// Convert Assimp::vec3 to glm::vec3
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		// Set position of each vertex
		vertex.position = temp;

		//  Set normal of each vertex
		if (mesh->HasNormals())
		{
			temp.x = mesh->mNormals[i].x;
			temp.y = mesh->mNormals[i].y;
			temp.z = mesh->mNormals[i].z;
			vertex.normal = temp;
		}

		// set texture coordinate of each vertex
		if (mesh->mTextureCoords[0])
		{
			vec2 texCoords_temp;

			texCoords_temp.x = mesh->mTextureCoords[0][i].x;
			texCoords_temp.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = texCoords_temp;
		}
		else
			vertex.texCoords = vec2(0.0f, 0.0f);
		
		m_vertices.push_back(vertex);
	}
	
	// Get indices of the mesh faces(triangle)
	for (unsigned int i = 0; i < num_faces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
		//cout << "Face" << i + 1 << " has " << face.mIndices[0] << " " << face.mIndices[1] << " " << face.mIndices[2] << endl;
 		m_indices.push_back(face.mIndices[0]);
		m_indices.push_back(face.mIndices[1]);
		m_indices.push_back(face.mIndices[2]);
	}

	// Load materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	
	m_material = LoadMaterial(material);

	// Load Diffuse from texture
	vector<TextureS> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
	m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// Load specular from texture
	vector<TextureS> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
	m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
	
	// Load normal map from texture
	vector<TextureS> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
	m_textures.insert(m_textures.end(), normalMaps.begin(), normalMaps.end());
	
	// Load height map from texture
	vector<TextureS> heightMaps = LoadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
	m_textures.insert(m_textures.end(), heightMaps.begin(), heightMaps.end());

	/*for (int i = 0; i < m_textures.size(); ++i)
	{
		cout << m_textures[i].path << " ";
	}
	cout << endl;*/
	return Mesh(m_vertices, m_indices, m_textures, m_material);
}

Material Model::LoadMaterial(aiMaterial* mat)
{
	//cout << "Load Material!" << endl;

	aiString str;
	Material material;

	if (AI_SUCCESS != mat->Get(AI_MATKEY_NAME, str))
	{
		// if material does not have any name
		// just return empty vector...
		cerr << "Material does not have any name..." << endl;
		return material;
	}

	//cout << "Material ready to load" << endl;
	//bool skip = false;

	//for (unsigned int i = 0; i < m_materials_loaded.size(); ++i)
	//{
	//	if (strcmp(m_materials_loaded[i].type.data(), str.C_Str()) == 0)
	//	{
	//		// if a material was loaded before
	//		// just push the material to 'materials' vector container
	//		materials.push_back(m_materials_loaded[i]);
	//		skip = true;
	//		break;
	//	}
	//}

	aiColor3D color(0.0f, 0.0f, 0.0f);



	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_AMBIENT, color))
	{
		material.ambient = { color.r, color.g, color.b };
		//cout << "Set ambient " << material.ambient.x << endl;
	}
	else
	{
		material.ambient = { 0.0f, 0.0f, 0.0f };
	}

	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_DIFFUSE, color))
	{
		material.diffuse = { color.r, color.g, color.b };
		//cout << "Set Diffuse" << material.diffuse.x << endl;
	}
	else
	{
		material.diffuse = { 0.0f, 0.0f, 0.0f };
	}

	if (AI_SUCCESS == mat->Get(AI_MATKEY_COLOR_SPECULAR, color))
	{
		material.specular = { color.r, color.g, color.b };
		//cout << "Set specular" << material.specular.x << endl;
	}
	else
	{
		material.specular = { 0.0f, 0.0f, 0.0f };
	}

	float metal_factor = 0.0;

	if (AI_SUCCESS == mat->Get(AI_MATKEY_SHININESS, metal_factor))
	{
		material.shininess = metal_factor;
		//cout << "Metallic Factor is " << material.shininess << endl;
		
	}

	return material;
}

vector<TextureS> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	//cout << "Load Material Texture " << typeName << " " << mat->GetTextureCount(type) << endl;

	vector<TextureS> textures;
	
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;

		//cout << "str: " << str.C_Str() << endl;

		// Check if the texture was loaded before
		for (unsigned int j = 0; j < m_textures_loaded.size(); ++j)
		{
			if (strcmp(m_textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				// If the texture was loaded before, 
				// just push the loaded texture to the textures container
				textures.push_back(m_textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			TextureS texture;
			texture.id = TextureFromFile(str.C_Str(), m_directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			m_textures_loaded.push_back(texture);
		}
	}
	
	return textures;
}

unsigned int Model::TextureFromFile(const char* path, const string& directory, bool gamma)
{

	string fileName = string(path);
	fileName = directory + '/' + fileName;
	//cout << "Load Texture! from " << fileName << endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	// Load a texture
	int width, height, num_channel;
	unsigned char* texture = stbi_load(fileName.c_str(), &width, &height, &num_channel, 0);
	if (texture)
	{
		GLenum format = GL_RED;
		if (num_channel == 1)
			format = GL_RED;
		else if (num_channel == 3)
			format = GL_RGB;
		else if (num_channel == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(texture);
	}
	else
	{
		cout << "Texture failed to load at path: " << path << endl;
		stbi_image_free(texture);
	}

	return textureID;
}


