#pragma once

#include <GL/glew.h>
#include <string>
#include "Math.h"

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Shader
{
public:
	Shader();
	~Shader();

	bool LoadShaderFile(string& vertPath, string& fragPath);
	void UnLoad();
	void SetActive();

	void SetInt(const string& name, int value) const;
	void SetFloat(const string& name, float value) const;
	void SetVec3(const string& name, vec3& vector) const;
	void SetVec3(const string& name, float x, float y, float z) const;
	void SetMat4(const string& name, mat4& matrix) const;


	GLuint GetShaderProgram() const { return mShaderProgram; };

private:
	bool CompileShader(const string& fileName, GLenum shaderType,
		GLuint& outShader);
	bool IsCompiled(GLuint& shader);

	GLuint mVertShader;
	GLuint mFragShader;
	GLuint mShaderProgram;

	string mVertPath;
	string mFragPath;
};