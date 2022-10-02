#include "Model.h"

#include <fstream>
#include <sstream>

using namespace std;
using namespace Assimp;

Model::Model(string const& path, bool gamma) 
		: m_path(path), m_gamma_correction(gamma), m_root_transformation(1.0f),
		  m_aiScene(), m_importer()
{}

void Model::Draw(Shader& shader)
{
	if (debug)
	{
		//cout << "The size of mesh is " << m_meshes.size() << endl;
		debug = false;
	}

	for (unsigned int i = 0; i < m_meshes.size(); ++i)
		m_meshes[i]->Draw(shader);
}

bool Model::LoadModel()
{
	cout << "Load model! " << m_path << endl;

	m_aiScene = m_importer.ReadFile(m_path, aiProcess_Triangulate |
									  aiProcess_GenSmoothNormals | 
									  aiProcess_CalcTangentSpace);
	
	if (!m_aiScene || m_aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_aiScene->mRootNode)
	{
		cout << "ERROR::ASSIMP:: " << m_importer.GetErrorString() << endl;
		assert(0);
		return false;
	}

	// The directory path of the file path
	m_directory = m_path.substr(0, m_path.find_last_of('/'));
	cout << "Directory: " << m_directory << endl;
	
	// Process ASSIMP's root node, and then recursively process its child node
	aiNode* root_node = m_aiScene->mRootNode;
	
	m_root_transformation = ConvertMatrixToGLMFormat(root_node->mTransformation);
	//cout << "Root node transformation: ";
	//cout << m_root_transformation << endl;

	ProcessNode(root_node, m_aiScene);
	//cout << scene.use_count() << endl;
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	cout << "Process aiNode " << node->mName.C_Str() << " meshes: " << node->mNumMeshes  << " childrens: " << node->mNumChildren << endl;
	
	// Node Transformation:
	// coordinates of vertices of meshes from a node --> model space coordinate
	// Ends up whole nodes placed at correct position 
	mat4 node_transformation = ConvertMatrixToGLMFormat(node->mTransformation);
	//cout << "Node transformation  : " << endl;
	//cout << node_transformation << endl;
	
	mat4 adjust = { 1.0f,  0.0f,  0.0f, 0.0f,
					0.0f,  0.0f,  -1.0f, 0.0f,
					0.0f,  1.0f, 0.0f, 0.0f,
					0.0f,  0.0f,  0.0f, 1.0f };

	// Loop through all mesh indices in the node and process the actual meshes in the scene object
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* ai_mesh = scene->mMeshes[node->mMeshes[i]];
		unique_ptr<Mesh> mesh = ProcessMesh(ai_mesh, scene, node->mName.C_Str(), node_transformation);
		mesh->SetupMesh();
		mesh->SetMeshTransform(adjust);
		m_meshes.push_back(move(mesh));
	}
	//cout << endl;
	// Process all the children node recursively for processing a mesh of each children node
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		aiNode* child_node = node->mChildren[i];
		ProcessNode(child_node, scene);
	}
}

unique_ptr<Mesh> Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, const string& name, mat4& m)
{
	//cout << m << endl;
	vector<VertexLayout> vertices;
	vector<unsigned int> indices;
	vector<shared_ptr<Texture>> textures;
	Material material;

	// Data for mesh
	unsigned int num_vertices = mesh->mNumVertices;
	unsigned int num_faces = mesh->mNumFaces;
	unsigned int num_indices = num_faces * 3;
	unsigned int num_bones = mesh->mNumBones;
	string mesh_name = mesh->mName.C_Str();
	//cout << "  Process aiMesh " << mesh_name << " vertices: " << num_vertices <<
	//	    " indices: " << num_indices << " bones: " << num_bones << endl;
	//
	//cout << "Transform to ";
	//cout << m << endl;

	vec3 temp;
	vec2 texCoords_temp;
	for (unsigned int i = 0; i < num_vertices; ++i)
	{
		VertexLayout vertex;
		
		// Convert Assimp::vec3 to glm::vec3
		temp.x = mesh->mVertices[i].x;
		temp.y = mesh->mVertices[i].y;
		temp.z = mesh->mVertices[i].z;
		
		vec4 pos = vec4(temp, 1.0f);
		
		pos = m * pos;
		// Set position of each vertex
		vertex.position = /*{ pos.x, pos.y, pos.z }; */temp;

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

			texCoords_temp.x = mesh->mTextureCoords[0][i].x;
			texCoords_temp.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = texCoords_temp;
		}
		else
			vertex.texCoords = vec2(0.0f, 0.0f);
		
		vertices.push_back(vertex);
	}
	
	// Get indices of the mesh faces(triangle)
	for (unsigned int i = 0; i < num_faces; ++i)
	{
		const aiFace& face = mesh->mFaces[i];
 		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}


	// Get bones of the mesh
	if (mesh->HasBones())
	{
		LoadBones(vertices, mesh, scene);
	}

	//cout << "  Num of loaded bones: " << m_bone_info_map.size() << endl;

	// Load materials
	shared_ptr<aiMaterial> ai_material = make_shared<aiMaterial>();
	ai_material->CopyPropertyList(ai_material.get(), scene->mMaterials[mesh->mMaterialIndex]);

	material = LoadMaterial(ai_material);

	// Load Diffuse from texture
	vector<shared_ptr<Texture>> diffuse_maps = LoadMaterialTextures(ai_material, aiTextureType_DIFFUSE, "texture_diffuse");
	textures.insert(textures.end(), make_move_iterator(diffuse_maps.begin()), make_move_iterator(diffuse_maps.end()));

	// Load specular from texture
	vector<shared_ptr<Texture>> specularMaps = LoadMaterialTextures(ai_material, aiTextureType_SPECULAR, "texture_specular");
	textures.insert(textures.end(), make_move_iterator(specularMaps.begin()), make_move_iterator(specularMaps.end()));
	
	// Load normal map from texture
	vector<shared_ptr<Texture>> normalMaps = LoadMaterialTextures(ai_material, aiTextureType_HEIGHT, "texture_normal");
	textures.insert(textures.end(), make_move_iterator(normalMaps.begin()), make_move_iterator(normalMaps.end()));
	
	// Load height map from texture
	vector<shared_ptr<Texture>> heightMaps = LoadMaterialTextures(ai_material, aiTextureType_AMBIENT, "texture_ambient");
	textures.insert(textures.end(), make_move_iterator(heightMaps.begin()), make_move_iterator(heightMaps.end()));

	return make_unique<Mesh>(name, vertices, indices, textures, material);
}

Material Model::LoadMaterial(shared_ptr<aiMaterial> mat)
{
	aiString str;
	Material material;

	if (AI_SUCCESS != mat->Get(AI_MATKEY_NAME, str))
	{
		// if material does not have any name
		// just return empty Material...
		cerr << "Material does not have any name..." << endl;
		return material;
	}

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

vector<shared_ptr<Texture>> Model::LoadMaterialTextures(shared_ptr<aiMaterial> mat, aiTextureType type, string typeName)
{
	//cout << "    Load Material Texture " << typeName << " " << mat->GetTextureCount(type) << endl;

	vector<shared_ptr<Texture>> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		string path = m_directory + '/' + str.C_Str();

		//cout << "     texture " << i << " path: " << path << endl;
		//cout << "     Loaded Texture: " << endl;
		// Check if the texture was loaded before
		for (unsigned int j = 0; j < m_textures_loaded.size(); ++j)
		{
			 cout << "     " << m_textures_loaded[j]->GetPath() << endl;
			if (strcmp(m_textures_loaded[j]->GetPath().data(), path.c_str()) == 0)
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
			shared_ptr<Texture> new_texture = make_shared<Texture>(path, typeName);
			assert(new_texture->LoadTexture() != false);
			textures.push_back(new_texture);
			m_textures_loaded.push_back(new_texture);
			//cout << " new texture: " << textures.back()->GetPath();
		}
	}
	//cout << endl;
	return textures;
}

bool Model::LoadBones(vector<VertexLayout>& vertices, aiMesh* mesh, const aiScene* scene)
{
	//cout << "    LoadBones for each Mesh " << endl;
	for (unsigned int i = 0; i < mesh->mNumBones; ++i)
	{
		// make_shared is ok, as aiBone has deep copy constructor
		shared_ptr<aiBone> ai_bone = make_shared<aiBone>(*mesh->mBones[i]);
		if (!LoadSingleBone(vertices, ai_bone, i))
		{
			cerr << "FAILED TO LOAD BONE: " << ai_bone->mName.C_Str() << endl;
			return false;
		}
	}
	//cout << endl;

	return true;
}

bool Model::LoadSingleBone(vector<VertexLayout>& vertices, shared_ptr<aiBone> bone, int bone_index)
{
	//cout << "     Bone Index " << bone_index << " ";
	string bone_name = bone->mName.C_Str();
	int bone_ID = GetBoneID(bone);
	if (bone_ID == -1)
	{
		cout << endl;
		cerr << "FAILED TO LOAD BONE " << bone_name << "'s ID: " << bone_ID << endl;
		return false;
	}

	int num_weights = bone->mNumWeights;
	//cout << bone_name << ":  weights: " << num_weights << " bone id: " << bone_ID << endl;
	
	auto matrix = ConvertMatrixToGLMFormat(bone->mOffsetMatrix);

	//cout << "      Bone Offset Matrix: " << matrix << endl;

	auto weights = bone->mWeights;
	for (int i = 0; i < num_weights; ++i)
	{
		int vertex_ID = weights[i].mVertexId;
		float weight = weights[i].mWeight;

		if (vertex_ID > vertices.size())
		{
			cerr << "VERTEX ID EXCEEDS THE SIZE OF VERTICES VECTOR: " << vertex_ID << endl;
			return false;
		}
		//cout << "      Add a bone to vertex layout " << vertex_ID;
		AddBoneInfo(vertices[vertex_ID], bone_ID, weight);
	}

	return true;
}

int Model::GetBoneID(const shared_ptr<aiBone> bone)
{
	int bone_ID = -1;
	string bone_name = bone->mName.C_Str();
	// If the bone was loaded previously
	if (m_bone_info_map.find(bone_name) == m_bone_info_map.end())
	{
		BoneInfo new_bone_info;
		new_bone_info.id = static_cast<int>(m_bone_info_map.size());
		new_bone_info.offset = ConvertMatrixToGLMFormat(bone->mOffsetMatrix);
		m_bone_info_map[bone_name] = new_bone_info;
		bone_ID = new_bone_info.id;
	}
	else
	{
		bone_ID = m_bone_info_map[bone_name].id;
	}

	return bone_ID;
}

void Model::AddBoneInfo(VertexLayout& vertex, int bone_ID, float weight)
{
	for (int i = 0; i < max_bone_per_vertex; ++i)
	{
		if (vertex.bone_IDs[i] == -1)
		{
			/*cout << " Vertex " << vertex.position.x << " " << vertex.position.y << " " << vertex.position.z << " ";
			cout << " added at " << i << " bone_ID: " << bone_ID << " weight: " << weight << endl;*/
			vertex.bone_IDs[i] = bone_ID;
			vertex.weights[i] = weight;
			return;
		}
	}
}

ostream& operator<<(ostream& os, aiMatrix4x4 m)
{
	return os << m.a1 << " " << m.a2 << " " << m.a3 << " " << m.a4 << " "
		<< m.b1 << " " << m.b2 << " " << m.b3 << " " << m.b4 << " "
		<< m.c1 << " " << m.c2 << " " << m.c3 << " " << m.c4 << " "
		<< m.d1 << " " << m.d2 << " " << m.d3 << " " << m.d4;
}

mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& aiMat)
{
	mat4 mat;

	mat[0][0] = aiMat.a1;
	mat[1][0] = aiMat.a2;
	mat[2][0] = aiMat.a3;
	mat[3][0] = aiMat.a4;

	mat[0][1] = aiMat.b1;
	mat[1][1] = aiMat.b2;
	mat[2][1] = aiMat.b3;
	mat[3][1] = aiMat.b4;

	mat[0][2] = aiMat.c1;
	mat[1][2] = aiMat.c2;
	mat[2][2] = aiMat.c3;
	mat[3][2] = aiMat.c4;

	mat[0][3] = aiMat.d1;
	mat[1][3] = aiMat.d2;
	mat[2][3] = aiMat.d3;
	mat[3][3] = aiMat.d4;

	return mat;
}

vec3 ConvertVectorToGLMFormat(const aiVector3D& aiVec)
{
	vec3 vec;

	vec.x = aiVec.x;
	vec.y = aiVec.y;
	vec.z = aiVec.z;

	return vec;
}

quat ConvertQuatToGLMFormat(const aiQuaternion& aiQuat)
{
	quat quat;

	quat.w = aiQuat.w;
	quat.x = aiQuat.x;
	quat.y = aiQuat.y;
	quat.z = aiQuat.z;

	return quat;
}