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

	if (!renderer->initialize(800.0f, 600.0f))
	{
		cout << "Failed to initialize!" << endl;
		return 0;
	}

	while (renderer->IsRunning())
	{
		renderer->Draw();
	}

	// Terminate and clear all allocated SDL resources
	renderer->UnLoad();

	return 0;
}