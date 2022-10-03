#include "Animator.h"

Animator::Animator(shared_ptr<Model> model) 
	: m_time(0.0f), m_delta_time(0.0f), m_base_name("")
{
	m_model = model;
	cout << "Animator Constructor" << endl;
	assert(m_model);
	
	const string path = m_model->GetPath();
	

	m_aiAnimation = m_model->GetScene()->mAnimations[0];
	//cout << m_aiAnimation->mChannels[2]->mNodeName.data << endl;
	
	m_duration = (float)m_aiAnimation->mDuration;
	m_ticks = (float)m_aiAnimation->mTicksPerSecond;
}

void Animator::UpdateAnimation(float dt)
{
	m_delta_time = dt*0.001f;
	//cout << "Delta: " << m_delta_time << endl;
	
	//Importer importer;
	//m_ai_scene = importer.ReadFile(m_model->GetPath(), aiProcess_Triangulate |
	//													aiProcess_GenSmoothNormals |
	//													aiProcess_CalcTangentSpace);

	UpdateTime();
	const aiNode* root_node = m_model->GetScene()->mRootNode;

	int n_bones = static_cast<int>(m_model->GetBoneInfoMap().size());
	m_matrices = vector<mat4>(n_bones, mat4(1.0f));
	//cout << "number of bones for animation: " << m_matrices.size() << endl;
	//cout << root_node->mChildren << endl;
	
	CalculateBoneTransform(root_node, mat4(1.0f));

	debug = false;
}

void Animator::UpdateTime()
{
	//cout << "Update Time " << m_duration << " " << m_ticks << endl;
	m_time += m_delta_time * m_ticks;
	//cout << "Update time: " << m_time << endl;
	//cout << "duration : " << m_animation->GetDuration() << endl;
	m_time = fmod(m_time, m_duration);
}

//void Animator::PlayAnimation(shared_ptr<Animation> animation)
//{
//	m_animation = animation;
//	m_time = 0.0f;
//}

void Animator::CalculateBoneTransform(const aiNode* node, mat4 parent_transform)
{
	//cout << "Hi" << endl;
	string node_name = node->mName.data;
	mat4 node_transform = ConvertMatrixToGLMFormat(node->mTransformation);
	//cout << "Node name: " << node_name << endl;

	const aiNodeAnim* nodeAnim = FindNodeAnim(node_name);
	//assert(nodeAnim);

	if (nodeAnim)
	{
		// If there is an animation 

		shared_ptr<BoneTransform> bone = make_shared<BoneTransform>(node_name, nodeAnim, m_time);
		node_transform = bone->GetLocalTransform();
	}

	//mat4 final_transform = parent_transform * node_transform;

	//cout << "With adjust: ";
	//cout << final_transform << endl;

	//final_transform *= adjust;

	BoneInfoMap& bone_info_map = m_model->GetBoneInfoMap();

	mat4 final_transform = parent_transform*node_transform;
	
	if (bone_info_map.find(node_name) != bone_info_map.end())
	{
		//cout << "POSE TRANSFORM: ";
		//cout << pose_transform << endl;

		int bone_ID = bone_info_map.at(node_name).id;
		mat4 bone_offset = bone_info_map.at(node_name).offset;
		//cout << "Bone name: " << node_name << " with ID: " << bone_ID << endl;
		
		//if (bone_ID == 0)
		//{
		//	parent_transform = mat4(1.0f);
		//}

		//mat4 bone_transform = final_transform * bone_offset;
		m_matrices.at(bone_ID) = final_transform * bone_offset;
		
		
		//cout << endl;
		//cout << "*" << endl;


		//cout << "vs" << endl;

		//cout << "Base transform: ";
		//cout << m_inverse_node_transform << endl;

		//bone_info_map.at(node_name).transformation = final_transform;
		
		//cout << endl;
		//cout << "Real good at " << bone_ID;
		//cout <<  good << endl;
	}

	if (debug)
	{
		cout << "Bone name: " << node_name << endl;
		/*cout << "Bone " << bone_ID << ": " << node_name << endl;*/
		cout << "Pose Transform: ";
		cout << node_transform << endl;

		cout << "Final transform: ";
		cout << final_transform << endl;

		//cout << "Bone offset: ";
		//cout << bone_offset << endl;

		cout << "Parent_transform: ";
		cout << parent_transform << endl;
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		CalculateBoneTransform(node->mChildren[i], final_transform);
	}
}

const aiNodeAnim* Animator::FindNodeAnim(const string& name)
{
	//cout << "Find nodeAnim " << m_model->GetScene()->mAnimations[0]->mNumChannels << endl;
	for (unsigned int i = 1; i < m_model->GetScene()->mAnimations[0]->mNumChannels; ++i)
	{
		const aiNodeAnim* nodeAnim = m_aiAnimation->mChannels[i];
	
		assert(nodeAnim);
		
		if (nodeAnim->mNodeName.data == name)
		{
			//cout << "Found it: " << nodeAnim->mNodeName.data << endl;
			return nodeAnim;
		}
	}

	return nullptr;
}




