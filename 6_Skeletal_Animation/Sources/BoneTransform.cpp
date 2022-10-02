#include "BoneTransform.h"

using namespace std;
using namespace glm;

BoneTransform::BoneTransform(const string& name, const aiNodeAnim* nodeAnim, float time)
	: m_name(name), m_time(time)
{
	//cout << "Bone Transform" << name << ": " << endl;

	mat4 T = InterpolatePosition(nodeAnim);
	mat4 R = InterpolateRotation(nodeAnim);
	mat4 S = InterpolateScale(nodeAnim);

	m_bone_transform = T * R * S;
}

//void BoneTransform::Update()
//{
//	mat4 t = InterpolatePosition(time);
//	mat4 r = InterpolateRotation(time);
//	mat4 s = InterpolateScale(time);
//
//	cout << "rotate?";
//	cout << r << endl;
//
//	m_pose_transform = t * r * s;
//}

mat4 BoneTransform::GetLocalTransform()
{
	return m_bone_transform;
}

string BoneTransform::GetBoneName() const
{
	return m_name;
}

//int BoneTransform::GetBoneID()
//{
//	return m_ID;
//}

int BoneTransform::GetPositionIndex(const aiNodeAnim* aiNode)
{
	for (unsigned int i = 0; i < aiNode->mNumPositionKeys-1; ++i)
	{
		float t = (float)aiNode->mPositionKeys[i + 1].mTime;
		//cout << m_rotations[next].m_time_stamp << endl;
		if (m_time < t) 
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

int BoneTransform::GetRotationIndex(const aiNodeAnim* aiNode)
{
	for (unsigned int i = 0; i < aiNode->mNumRotationKeys - 1; ++i)
	{
		float t = (float)aiNode->mRotationKeys[i + 1].mTime;
		//cout << m_rotations[next].m_time_stamp << endl;
		if (m_time < t)
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

int BoneTransform::GetScaleIndex(const aiNodeAnim* aiNode)
{
	for (unsigned int i = 0; i < aiNode->mNumScalingKeys - 1; ++i)
	{
		float t = (float)aiNode->mScalingKeys[i + 1].mTime;
		//cout << m_rotations[next].m_time_stamp << endl;
		if (m_time < t)
		{
			return i;
		}
	}

	assert(0);
	return -1;
}

float BoneTransform::GetInterpolateFactor(float t0, float t1)
{
	//cout << "  Get Interpolate Factor: ";
	//cout << "t0: " << t0 << " " << "t1: " << t1 << " time: " << time << " ";
	float dt = t1 - t0;
	float factor = (m_time - t0) / dt;

	//cout << "factor: " << factor << endl;

	assert(factor >= 0.0f && factor <= 1.0f);

	return factor;
}


mat4 BoneTransform::InterpolatePosition(const aiNodeAnim* nodeAnim)
{
	//cout << "Interpolate Position: ";

	if (nodeAnim->mNumPositionKeys == 1)
	{
		return translate(mat4(1.0f), ConvertVectorToGLMFormat(nodeAnim->mPositionKeys[0].mValue));
	}

	int index0 = GetPositionIndex(nodeAnim);
	int index1 = index0 + 1;
	
	//cout << "Index0: " << index0 << " Index1: " << index1 << " " << endl;

	float t0 = (float)nodeAnim->mPositionKeys[index0].mTime;
	float t1 = (float)nodeAnim->mPositionKeys[index1].mTime;
	float factor = GetInterpolateFactor(t0, t1);

	const vec3& pos_t0 = ConvertVectorToGLMFormat(nodeAnim->mPositionKeys[index0].mValue);
	const vec3& pos_t1 = ConvertVectorToGLMFormat(nodeAnim->mPositionKeys[index1].mValue);

	vec3 pos = mix(pos_t0, pos_t1, factor);

	return translate(mat4(1.0f), pos);
}

mat4 BoneTransform::InterpolateScale(const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumScalingKeys == 1)
	{
		return scale(mat4(1.0f), ConvertVectorToGLMFormat(nodeAnim->mScalingKeys[0].mValue));
	}

	int index0 = GetPositionIndex(nodeAnim);
	int index1 = index0 + 1;

	float t0 = (float)nodeAnim->mScalingKeys[index0].mTime;
	float t1 = (float)nodeAnim->mScalingKeys[index1].mTime;

	float factor = GetInterpolateFactor(t0, t1);

	const vec3& scale_t0 = ConvertVectorToGLMFormat(nodeAnim->mScalingKeys[index0].mValue);
	const vec3& scale_t1 = ConvertVectorToGLMFormat(nodeAnim->mScalingKeys[index1].mValue);

	vec3 sc = mix(scale_t0, scale_t1, factor);

	return scale(mat4(1.0f), sc);
}

mat4 BoneTransform::InterpolateRotation(const aiNodeAnim* nodeAnim)
{
	if (nodeAnim->mNumRotationKeys  == 1)
	{
		 quat rotation = normalize(ConvertQuatToGLMFormat(nodeAnim->mRotationKeys[0].mValue));
		return toMat4(rotation);
	}

	int index0 = GetRotationIndex(nodeAnim);
	int index1 = index0 + 1;

	float t0 = (float)nodeAnim->mRotationKeys[index0].mTime;
	float t1 = (float)nodeAnim->mRotationKeys[index1].mTime;

	float factor = GetInterpolateFactor(t0, t1);	
	
	const quat& rot_t0 = ConvertQuatToGLMFormat(nodeAnim->mRotationKeys[index0].mValue);
	const quat& rot_t1 = ConvertQuatToGLMFormat(nodeAnim->mRotationKeys[index1].mValue);

	quat rotation = normalize(slerp(rot_t0, rot_t1, factor));

	return toMat4(rotation);
}


