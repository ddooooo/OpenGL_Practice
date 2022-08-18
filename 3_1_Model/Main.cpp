#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Light.h"
#include "PointLight.h"
#include "SpotLight.h"

using namespace std;
using namespace glm;

int main(int argc, char** argv)
{

	Renderer* renderer = new Renderer();

	renderer->Initialize(800.0f, 600.0f);

	renderer->LoadVertices();

	renderer->LoadModel("Models/Link/Link_Final.obj", false);

	renderer->LoadShader();

	while (renderer->IsRunning())
	{
		renderer->Draw();
	}

	renderer->GetShader()->UnLoad();
	renderer->GetLightShader()->UnLoad();

	glDeleteVertexArrays(1, &renderer->m_light_VAO);

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(renderer->GetContext());
	SDL_DestroyWindow(renderer->GetWindow());
	SDL_Quit();

	return 0;
}