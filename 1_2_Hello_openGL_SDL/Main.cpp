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

using namespace std;
using namespace glm;

int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = SDL_CreateWindow("Hello SDL + OpenGL",
											SDL_WINDOWPOS_UNDEFINED,
											SDL_WINDOWPOS_UNDEFINED,
											1024,
											768,
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

	//GLuint vertShader;
	//GLuint fragShader;
	//GLuint shaderProgram = glCreateProgram();;

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
	unsigned int VAO, VBO, EBO;
	
	// Create and bind a vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create a vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCube), verticesCube, GL_STATIC_DRAW);
	
	// Create a element buffer
	glGenBuffers(1, &EBO);
	cout << "EBO: " << EBO << endl;
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

	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				running = 0;
			}
		}
		glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		

		//texture1->SetActive();
		//texture2->SetActive();

		// First Texture 
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
		
		// Second Texture
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());

		// Set up Model Matrix (Transformation Matrix) 
		mat4 model = mat4(1.0f); // Create a identity matrix
		// Scale -> Rotate -> Translate
		model = translate(model, vec3(0.5f, -0.5f, 0.0f)); // 3. Translate
		// GLM will automatically multiply the matrices together
		float time = SDL_GetTicks() * 0.001; 
		model = rotate(model, time, vec3(0.0f, 0.0f, 1.0f)); // 2. Rotate
		model = rotate(model, radians(-55.0f), vec3(1.0f, 0.0f, 0.0f));
		model = scale(model, vec3(0.5f, 0.5f, 0.5f)); // 1. Scale

		// Set up View Matrix
		mat4 view = mat4(1.0f);
		view = translate(view, vec3(0.0f, 0.0f, -3.0f));
		
		// Set up Projection Matrix
		mat4 projection = mat4(1.0f);
		projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

		unsigned int modelLoc = glGetUniformLocation(shader->getShaderProgram(), 
													 "model");
		unsigned int viewLoc = glGetUniformLocation(shader->getShaderProgram(),
													"view");
		unsigned int projLoc = glGetUniformLocation(shader->getShaderProgram(),
													"projection");

		// Assign matrix to variables in GLSL Program
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

		// Draw a rectangle 
		//float timeValue = SDL_GetTicks() * 0.01;
		//float blueValue = sin(timeValue)/2.0f + 0.5f;
		//int vertexColor = glGetUniformLocation(shaderProgram, "ourColor");
		//glUniform4f(vertexColor, 0.0f, 0.0f, blueValue, 1.0f);

		shader->SetActive();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
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