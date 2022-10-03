#pragma once
#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Model.h"
#include "BoneTransform.h"

class Animator
{
private:
	shared_ptr<Model> m_model;

	vector<mat4> m_matrices;

	string m_base_name;

	float m_time;
	float m_delta_time;
	float m_duration;
	float m_ticks;

	const aiAnimation* m_aiAnimation;

	bool debug = true;
public:
	Animator(shared_ptr<Model> model);
	
	vector<mat4>& GetTransforms() { return m_matrices; }

	void UpdateAnimation(float dt);
	//void PlayAnimation(shared_ptr<Model> animation);
	void CalculateBoneTransform(const aiNode* node, mat4 parent_transform);
private:
	void UpdateTime();
	const aiNodeAnim* FindNodeAnim(const string& name);
};

#endif ANIMATOR_H
