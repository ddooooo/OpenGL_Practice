#pragma once

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>

using namespace std;

enum class ModelName
{
	LINK = 0,
	SIMPLE,
	SIMPLE2,
	BOX,
	TEST,
	CHARACTER,
};

static string enumTostring[] = 
{ 
	"Models/Link/Link.fbx", 
	"Models/Simple.fbx", 
	"Models/Simple2.fbx",
	"Models/Box.fbx",
	"Models/Test.fbx",
	"Models/Character.fbx",
};

//mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& aiMat)
//{
//	mat4 mat;
//
//	mat[0][0] = aiMat.a1;
//	mat[1][0] = aiMat.a2;
//	mat[2][0] = aiMat.a3;
//	mat[3][0] = aiMat.a4;
//
//	mat[0][1] = aiMat.b1;
//	mat[1][1] = aiMat.b2;
//	mat[2][1] = aiMat.b3;
//	mat[3][1] = aiMat.b4;
//
//	mat[0][2] = aiMat.c1;
//	mat[1][2] = aiMat.c2;
//	mat[2][2] = aiMat.c3;
//	mat[3][2] = aiMat.c4;
//
//	mat[0][3] = aiMat.d1;
//	mat[1][3] = aiMat.d2;
//	mat[2][3] = aiMat.d3;
//	mat[3][3] = aiMat.d4;
//
//	return mat;
//}
//
//vec3 ConvertVectorToGLMFormat(const aiVector3D& aiVec)
//{
//	vec3 vec;
//
//	vec.x = aiVec.x;
//	vec.y = aiVec.y;
//	vec.z = aiVec.z;
//
//	return vec;
//}
//
//quat ConvertQuatToGLMFormat(const aiQuaternion& aiQuat)
//{
//	quat quat;
//
//	quat.w = aiQuat.w;
//	quat.x = aiQuat.x;
//	quat.y = aiQuat.y;
//	quat.z = aiQuat.z;
//
//	return quat;
//}


#endif // !CONFIG_H
