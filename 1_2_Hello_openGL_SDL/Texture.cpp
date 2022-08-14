#include "Texture.h"

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture() : mTexture(0), mWidth(0), mHeight(0)
{
}

Texture::~Texture()
{
}

bool Texture::Load(const string& fileName)
{
	int nChannels = 0;
	stbi_set_flip_vertically_on_load(true); // Flip the image upside-down
	unsigned char* image = stbi_load(fileName.c_str(), &mWidth, &mHeight, 
										&nChannels, 0);
	if (!image) 
	{
		printf("stbi_load failed to load image %s and %s \n", 
					fileName.c_str());
		return false;
	}

	int format = GL_RGB;
	if (nChannels == 4) 
	{
		format = GL_RGBA;
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, mWidth, mHeight, 0, 
					format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);

	// Set texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return true;
}

void Texture::Unload()
{
	glDeleteTextures(1, &mTexture);
}

void Texture::SetActive()
{
	glBindTexture(GL_TEXTURE_2D, mTexture);
}



