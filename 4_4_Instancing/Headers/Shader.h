#pragma once

#include <GL/glew.h>
#include <string>
#include "Math.h"
#include <iostream>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

class Shader
{
public:
	Shader();
	~Shader();

	bool LoadShaderFile(const string& vert_path, const string& frag_path, const string& geom_path = "");
	void UnLoad();
	void SetActive();

	void SetInt(const string& name, int value) const;
	void SetFloat(const string& name, float value) const;
	void SetVec3(const string& name, vec3 vector) const;
	void SetVec3(const string& name, float x, float y, float z) const;
	void SetMat4(const string& name, mat4& matrix) const;


	GLuint GetShaderProgram() const { return m_shader_ID; };

private:
	bool CompileShader(const string& fileName, GLenum shaderType,
		GLuint& outShader);
	bool IsCompiled(GLuint& shader);

	GLuint m_vert_shader;
	GLuint m_frag_shader;
	GLuint m_geom_shader;
	GLuint m_shader_ID;
};