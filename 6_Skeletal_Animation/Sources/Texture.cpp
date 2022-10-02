#include "Texture.h"

#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture() : m_texture_ID(0), m_width(0), m_height(0), m_path("")
{
}

Texture::Texture(const string& path) : m_texture_ID(0), m_width(0), m_height(0)
{
	m_path = path;
}

Texture::Texture(const string& path, const string& type) : m_texture_ID(0), m_width(0), m_height(0)
{
	m_path = path;
	m_type = type;
}


Texture::~Texture()
{
}

bool Texture::LoadTexture()
{
	int n_channel = 0;
	
	//cout << "    Load Texture from " << m_path << " type: " << m_type << endl;

	stbi_set_flip_vertically_on_load(true); // Flip the image upside-down
	
	unsigned char* texture = stbi_load(m_path.c_str(), &m_width, &m_height, &n_channel, 0);

	if (!texture)
	{
		cout << "Texture failed to load at path: " << m_path << endl;
		stbi_image_free(texture);
		return false;
	}

	int format = GL_RGB;
	if (n_channel == 4)
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &m_texture_ID);
	glBindTexture(GL_TEXTURE_2D, m_texture_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, texture);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Set texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (glGetError() != GL_NO_ERROR)
	{
		cerr << "TEXTURE ERROR FROM " << m_path << " " << m_type << endl;
		return false;
	}

	stbi_image_free(texture);

	return true;
}

bool Texture::LoadCubemapTexture(const vector<string>& faces)
{
	glGenTextures(1, &m_texture_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_ID);

	int n_channel = 0;
	for (int i = 0; i < faces.size(); ++i)
	{
		unsigned char* texture = stbi_load(faces[i].c_str(), &m_width, &m_height, &n_channel, 0);
		std::cout << "Load Texture! from " << i << " " << faces[i].c_str() << std::endl;
		if (!texture)
		{
			printf("stbi_load failed to load image %s \n", faces[i].c_str());
			stbi_image_free(texture);
			return false;
		}

		int format = GL_RGB;
		if (n_channel == 4)
		{
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, texture);
		stbi_image_free(texture);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &m_texture_ID);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, m_texture_ID);
}