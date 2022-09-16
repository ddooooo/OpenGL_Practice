#include "ShadowCubemapFBO.h"

ShadowCubemapFBO::ShadowCubemapFBO(unsigned int width, unsigned int height) 
	: m_FBO(0), m_cubemap(0), m_width(width), m_height(height)
{}

bool ShadowCubemapFBO::Init()
{
	cout << "Generate Shadow framebuffer" << endl;

	// Create cubemap texture to store shadow values 
	glGenTextures(1, &m_cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemap);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	
	// Create a six faces for cubemap texture
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	// Framebuffer to store depth values(shadow map)
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_cubemap, 0);

	// Disable writes to color buffer, as shadow map will not output the color
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glGetError() != GL_NO_ERROR)
	{
		cerr << "Error " << glGetError() << endl;
		return false;
	}

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		cerr << "Framebuffer error " << status << endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowCubemapFBO::BindFramebuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
}

void ShadowCubemapFBO::BindFrameTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_cubemap);
}


