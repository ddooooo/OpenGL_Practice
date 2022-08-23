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
	Renderer::Shape a = Renderer::Shape::BOX;

	Renderer* renderer = new Renderer();

	renderer->Initialize(800.0f, 600.0f);

	renderer->LoadPrimitive(Renderer::Shape::CUBE);

	renderer->LoadPrimitive(Renderer::Shape::BOX);

	if (!renderer->LoadModel("Models/Link/Link_Final.obj", false))
	{
		cout << "Failed to load model" << endl;
		return 0;
	}

	if (!renderer->LoadShader())
	{
		cout << "Failed to load shaders" << endl;
		return 0;
	}

	while (renderer->IsRunning())
	{
		renderer->Draw();
	}

	//renderer->GetShader()->UnLoad();
	//renderer->GetLightShader()->UnLoad();

	//glDeleteVertexArrays(1, &renderer->m_light_VAO);

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(renderer->GetContext());
	SDL_DestroyWindow(renderer->GetWindow());
	SDL_Quit();

	return 0;
}