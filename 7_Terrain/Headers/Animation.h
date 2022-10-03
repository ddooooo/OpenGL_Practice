#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include "Model.h"
#include "BoneTransform.h"

using namespace std;
using namespace glm;
struct NodeAnim
{
	mat4 m_transformation = mat4(1.0f);
	string m_name = "";
	int n_children = 0;

	vector<NodeAnim> m_children = {};
};

class Animation
{
private:
	shared_ptr<Model> m_model;
	shared_ptr<NodeAnim> m_root_node;
	
	vector<shared_ptr<BoneTransform>> m_bone_transforms;

	float m_duration;
	int m_ticks_per_second;

public:
	Animation(shared_ptr<Model> model);
	~Animation();

	shared_ptr<BoneTransform> FindBone(const string& name);

	// Get functions
	inline float GetDuration() { return m_duration; }
	inline int GetTicksPerSecond() { return m_ticks_per_second; }
	inline const NodeAnim& GetRootNode() { return *m_root_node; }
	BoneInfoMap& GetBoneInfoMap() { return m_model->GetBoneInfoMap(); }
	
private:
	void ProcessBoneTransforms(const aiAnimation* animation);
	void ProcessAnimNodes(NodeAnim& node, const aiNode* ai_node);
	void TraverseNode(NodeAnim& node, int count);
};

#endif ANIMATION_H
