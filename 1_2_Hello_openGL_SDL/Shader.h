#pragma once

#include <GL/glew.h>
#include <string>
#include "Math.h"

using namespace std;

class Shader
{
public:
	Shader();
	~Shader();

	bool LoadShaderFile(string& vertPath, string& fragPath);
	void UnLoad();
	void SetActive();

	void SetInt(const string& name, int value) const;

	GLuint getShaderProgram() const { return mShaderProgram; };

private:
	bool CompileShader(const string& fileName, GLenum shaderType,
						GLuint& outShader);
	bool IsCompiled(GLuint& shader);

	GLuint mVertShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};