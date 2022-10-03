#pragma once

#ifndef BONE_H
#define BONE_H

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Model.h"
#include "Mesh.h"
#include "Config.h"

class BoneTransform
{
private:
	mat4 m_bone_transform;
	string m_name;
	float m_time;

public:
	BoneTransform(const string& name, const aiNodeAnim* aiNode, float time);
	
	//void Update();

	mat4 GetLocalTransform();
	string GetBoneName() const;
	//int GetBoneID();

	int GetPositionIndex(const aiNodeAnim* aiNode);
	int GetRotationIndex(const aiNodeAnim* aiNode);
	int GetScaleIndex(const aiNodeAnim* aiNode);

private:
	float GetInterpolateFactor(float last, float next);
	
	mat4 InterpolatePosition(const aiNodeAnim* aiNode);
	mat4 InterpolateScale(const aiNodeAnim* aiNode);
	mat4 InterpolateRotation(const aiNodeAnim* aiNode);
};

#endif BONE_H