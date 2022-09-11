#include "Shader.h"

#include <fstream>
#include <sstream>

Shader::Shader() : m_vert_shader(0), m_frag_shader(0), m_geom_shader(0), m_shader_ID(0)
{
}

Shader::~Shader()
{
}

bool Shader::LoadShaderFile(const string& vert_path, const string& frag_path, const string& geom_path)
{
	if (!CompileShader(vert_path, GL_VERTEX_SHADER, m_vert_shader) ||
		!CompileShader(frag_path, GL_FRAGMENT_SHADER, m_frag_shader ))
	{
		return false;
	}

	if (geom_path != "")
	{
		if (!CompileShader(geom_path, GL_GEOMETRY_SHADER, m_geom_shader))
		{
			return false;
		}
	}

	m_shader_ID = glCreateProgram();

	glAttachShader(m_shader_ID, m_vert_shader);
	glAttachShader(m_shader_ID, m_frag_shader);

	if (geom_path != "")
	{
		glAttachShader(m_shader_ID, m_geom_shader);
	}

	glLinkProgram(m_shader_ID);

	GLint success;
	glGetProgramiv(m_shader_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		memset(infoLog, 0, 512);
		glGetProgramInfoLog(m_shader_ID, 512, nullptr, infoLog);
		printf("Compile Failed: %s", infoLog);
		return false;
	}

	glDeleteShader(m_vert_shader);
	glDeleteShader(m_frag_shader);

	if (geom_path != "")
	{
		glDeleteShader(m_geom_shader);
	}

	return true;
}

bool Shader::CompileShader(const string& fileName, GLenum shaderType, GLuint& outShader)
{
	printf("Compile %s \n", fileName.c_str());

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
			printf("Failed to compile shader %s \n", fileName.c_str());
			return false;
		}

		shaderFile.close();
	}
	else
	{
		printf("Shader file not found! %s \n", fileName.c_str());
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

void Shader::UnLoad()
{
	glDeleteProgram(m_shader_ID);
}

void Shader::SetActive()
{
	glUseProgram(m_shader_ID);
}

void Shader::SetInt(const string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_shader_ID, name.c_str()), value);
}

void Shader::SetFloat(const string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_shader_ID, name.c_str()), value);
}

void Shader::SetVec3(const string& name, vec3 vector) const
{
	glUniform3fv(glGetUniformLocation(m_shader_ID, name.c_str()), 1, &vector[0]);
}

void Shader::SetVec3(const string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(m_shader_ID, name.c_str()), x, y, z);
}

void Shader::SetMat4(const string& name, mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_shader_ID, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}