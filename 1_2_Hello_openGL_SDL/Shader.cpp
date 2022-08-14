#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader() : mVertShader(0), mFragShader(0), mShaderProgram(0)
{
}

Shader::~Shader()
{
}

bool Shader::LoadShaderFile(string& vertPath, string& fragPath)
{
	if (!CompileShader(vertPath, GL_VERTEX_SHADER, mVertShader) ||
		!CompileShader(fragPath, GL_FRAGMENT_SHADER, mFragShader))
	{
		return false;
	}

	mShaderProgram = glCreateProgram();

	glAttachShader(mShaderProgram, mVertShader);
	glAttachShader(mShaderProgram, mFragShader);

	glLinkProgram(mShaderProgram);

	GLint success;
	glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		memset(infoLog, 0, 512);
		glGetProgramInfoLog(mShaderProgram, 512, nullptr, infoLog);
		printf("Compile Failed: %s", infoLog);
		return false;
	}

	glDeleteShader(mVertShader);
	glDeleteShader(mFragShader);

	return true;
}

void Shader::UnLoad()
{
	glDeleteProgram(mShaderProgram);
}

void Shader::SetActive()
{
	glUseProgram(mShaderProgram);
}

void Shader::SetInt(const string& name, int value) const
{
	glUniform1i(glGetUniformLocation(mShaderProgram, name.c_str()), value);
}

bool Shader::CompileShader(const string& fileName, GLenum shaderType, GLuint& outShader)
{
	ifstream shaderFile(fileName);
	if (shaderFile.is_open())
	{
		stringstream sstream;
		// Returns a pointer to the stream buffer object currently associated with the stream
		sstream << shaderFile.rdbuf();
		string contents = sstream.str();
		const char* contentsChar = contents.c_str();

		outShader = glCreateShader(shaderType); // Information of shader file

		glShaderSource(outShader, 1, &contentsChar, nullptr);
		glCompileShader(outShader);

		if (!IsCompiled(outShader))
		{
			printf("Failed to compile shader %s", fileName.c_str());
			return false;
		}

		shaderFile.close();
	}
	else
	{
		printf("Shader file not found! %s", fileName.c_str());
		return false;
	}

	return true;
}

bool Shader::IsCompiled(GLuint& shader)
{
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		memset(buffer, 0, 512);
		glGetShaderInfoLog(shader, 511, nullptr, buffer);
		printf("GLSL Complile Failed: %s", buffer);
		return false;
	}
	return true;
}


