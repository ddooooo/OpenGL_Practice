#include "Texture.h"

#include <GL/glew.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Texture::Texture() : m_texture_ID(0), m_width(0), m_height(0)
{
}

Texture::Texture(const string& path) : m_texture_ID(0), m_width(0), m_height(0)
{
	m_path = path;
}

Texture::~Texture()
{
}

bool Texture::LoadTexture()
{
	int n_channel = 0;
	
	std::cout << "Load Texture! from " << m_path << std::endl;

	//stbi_set_flip_vertically_on_load(true); // Flip the image upside-down
	
	unsigned char* texture = stbi_load(m_path.c_str(), &m_width, &m_height, &n_channel, 0);

	if (!texture)
	{
		printf("stbi_load failed to load image %s \n", m_path.c_str());
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

	stbi_image_free(texture);

	// Set texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

bool Texture::LoadCubemapTexture(const vector<string>& faces)
{
	glGenTextures(1, &m_texture_ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_ID);

	int n_channel = 0;
	for (int i = 0; i < faces.size(); ++i)
	{
		std::cout << "Load Texture! from " << faces[i].c_str() << std::endl;

		unsigned char* texture = stbi_load(faces[i].c_str(), &m_width, &m_height, &n_channel, 0);
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