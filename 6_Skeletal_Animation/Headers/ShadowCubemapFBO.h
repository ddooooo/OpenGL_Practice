#pragma once

#include <iostream>
#include <GL/glew.h>
#include <GL/GLU.h>

using namespace std;

class ShadowCubemapFBO
{
public:
	ShadowCubemapFBO(unsigned int width, unsigned int height);

	bool Init();

	void BindFramebuffer();
	void BindFrameTexture();


private:
	unsigned int m_FBO;
	unsigned int m_cubemap;

	unsigned int m_width;
	unsigned int m_height;
};