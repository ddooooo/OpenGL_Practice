#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

using namespace std;
using namespace glm;

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Hello SDL + OpenGL",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		800,
		600,
		SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
		SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GLContext context = SDL_GL_CreateContext(window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW.");
	}

	// On some platforms, GLEW will emit a benign error code,
	// so clear it
	glGetError();

	glEnable(GL_DEPTH_TEST);

	string vertPath = "Shaders/Coordinate_System.vert";
	string fragPath = "Shaders/MultiTexture.frag";
	string imagePath1 = "Textures/Container.jpg";
	string imagePath2 = "Textures/Awesomeface.png";

	// Create a new shader 
	Shader* shader = new Shader();

	// Create a new program object to store both vertex shader and fragment shader
	if (!shader->LoadShaderFile(vertPath, fragPath))
	{
		printf("Failed to load shader file! \n");
	}

	float vertices[] = {
		 0.5f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f
	};

	float verticesColor[] = {
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,	  0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	float verticesTexture[] = {
		// Object position		// color			// Texture Coordinate(UV)
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,	  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	   -0.5f, -0.5f, 0.0f,	  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
	};

	float verticesTransform[] = {
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
	};

	float verticesCube[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// Assign Vertex Buffer Object, Vertex Array Object, Element Buffer Object(Index)
	unsigned int VAO, VBO;

	// Create and bind a vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create a vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);

	// Create a element buffer
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Specify vertex Attributes
	/*glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);*/

	/*glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3*sizeof(float),
						  reinterpret_cast<void*>(sizeof(float) * 3));

	glBindVertexArray(VAO);*/

	// Position Attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);

	// Color Attribute
	/*glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
							(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);*/

	// Texture Attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
		(void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Load Texture1
	Texture* texture1 = new Texture();
	texture1->Load(imagePath1);

	// Load Texture2
	Texture* texture2 = new Texture();
	texture2->Load(imagePath2);

	shader->SetActive();
	shader->SetInt("texture1", 0);
	shader->SetInt("texture2", 1);

	SDL_Event event;
	int running = 1;
	printf("%s\n", glGetString(GL_VERSION));

	vec3 cubePositions[] = {
		vec3(0.0f,  0.0f,  0.0f),
		vec3(3.0f,  0.0f, 0.0f),
		vec3(1.5f, 1.5f, 0.0f),
		vec3(-3.0f, 0.0f, 0.0f),
		vec3(-1.5f, 1.5f, 0.0f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f, -2.0f, -2.5f),
		vec3(1.5f,  2.0f, -2.5f),
		vec3(1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

	Camera* camera = new Camera();

	while (running)
	{
		float currentFrame = (float)SDL_GetTicks() * 0.001f;
		float lastFrame = camera->GetLastFrame();
		camera->SetDeltaTime(currentFrame - lastFrame);
		camera->SetLastFrame(currentFrame);

		camera->ProcessInput();
		
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = 0;
			}
			else if (event.type == SDL_MOUSEMOTION)
			{
				camera->ProcessMouseMotion(event);
			}
		}
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//texture1->SetActive();
		//texture2->SetActive();

		// First Texture 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());

		// Second Texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());

		// Set up View Matrix (World space to view(camera) space)
		mat4 view = camera->MyLookAt();
			
			/*lookAt(camera->GetPos(),
			camera->GetForward() + camera->GetPos(),
			camera->GetUp());
		*/
		//if (camera->MyLookAt() == view) printf("---------------------\n");

		//camera->MyLookAt();
		shader->SetMat4("view", view);

		// Set up Projection Matrix (View Space to Clip Space 
		// (Clipped by frustrum)
		mat4 projection = perspective(radians(camera->GetFov()), 
									800.0f / 600.0f, 0.1f, 100.0f);
		shader->SetMat4("projection", projection);

		shader->SetActive();
		glBindVertexArray(VAO);

		// Set up Model Matrix (Local to World Coordinate)
		mat4 model = mat4(1.0f); 
		model = translate(model, cubePositions[0]);
		shader->SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		for (unsigned int i = 1; i < 10; i++) {
			model = mat4(1.0f);
			//float timeModel = (float)SDL_GetTicks() * 0.001f;
			//model = rotate(model, timeModel, vec3(-1.0f, 1.0f, 0.0f));
			model = translate(model, cubePositions[i]);
			shader->SetMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		/*glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);*/

		SDL_GL_SwapWindow(window);
	}

	// Deallocate all resources
	shader->UnLoad();
	texture1->Unload();
	texture2->Unload();

	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	// Free all dynamic memories
	delete shader;
	delete texture1;
	delete texture2;

	return 0;
}