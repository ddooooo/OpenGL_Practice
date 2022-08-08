#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	// Cube Position
	vec3 cubePositions[] = {
		vec3(0.0f,  0.0f,  0.0f),
		vec3(2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f, -2.0f, -2.5f),
		vec3(1.5f,  2.0f, -2.5f),
		vec3(1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};

	// Point lights
	vec3 lightPositions[] = {
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f)
	};


	Shader* shader = new Shader();

	string vertPath = "Shaders/6_1_MultiLights.vert";
	string fragPath = "Shaders/6_1_MultiLights.frag";

	if (!shader->LoadShaderFile(vertPath, fragPath))
	{
		printf("Failed to load shader files! \n");
	}

	Shader* lightShader = new Shader();

	string lightVertPath = "Shaders/1_LightColor.vert";
	string lightFragPath = "Shaders/1_LightColor.frag";

	if (!lightShader->LoadShaderFile(lightVertPath, lightFragPath))
	{
		printf("Failed to load shader files! \n");
	}

	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	// Position Attiribute 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normal Attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 
						  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture Coordinate Attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Camera* camera = new Camera();
	vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

	string texturePath1 = "Textures/Container2.png";

	Texture* texture1 = new Texture();
	if (texture1->Load(texturePath1))
	{
		printf("Texture Loaded \n");
	}

	string texturePath2 = "Textures/Container2_Specular.png";
	Texture* texture2 = new Texture();
	if (texture2->Load(texturePath2))
	{
		printf("Texture2 Loaded \n");
	}

	string texturePath3 = "Textures/Matrix.jpg";
	Texture* texture3 = new Texture();
	if (texture3->Load(texturePath3)) 
	{
		printf("Textuer3 Loaded \n");
	}


	shader->SetActive();
	shader->SetInt("material.diffuse", 0);
	shader->SetInt("material.specular", 1);
	shader->SetInt("material.emission", 2);

	SDL_Event event;
	int running = 1;
	printf("%s \n", glGetString(GL_VERSION));

	// Set up Lights

	// Directional Light
	vec3 amb = { 0.05f, 0.05f, 0.05f };
	vec3 diff = { 0.4f, 0.4f, 0.4f };
	vec3 spec = { 0.5f, 0.5f, 0.5f };
	vec3 dir = { -0.2f, -1.0f, -0.3f };
	Light dirLight( amb, diff, spec, dir);

	// Point Lights
	//amb = { 0.05f, 0.05f, 0.05f };
	diff = { 0.8f, 0.8f, 0.8f };
	spec = { 1.0f, 1.0f, 1.0f };
	float constant = 1.0f;
	float linear = 0.09f;
	float quadratic = 0.032f;

	PointLight* pointLights = new PointLight[4];

	for (int i = 0; i < 4; ++i)
	{
		pointLights[i] = PointLight(amb, diff, spec, lightPositions[i], constant, linear, quadratic);
	}

	amb = vec3(0.1f);
	diff = vec3(0.8f);
	spec = vec3(1.0f);
	float inner = cos(radians(12.5f));
	float outer = cos(radians(17.5f));

	SpotLight spotLight(amb, diff, spec, constant, linear, quadratic, inner, outer);

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

		vec3 camPos = camera->GetPos();
		vec3 camForward = camera->GetForward();
		
		shader->SetActive();
		shader->SetVec3("viewPos", camPos);
		shader->SetFloat("material.shininess", 32.0f);


		// Directional Light
		shader->SetVec3("dirLight.direction", dirLight.GetDir().x, dirLight.GetDir().y, dirLight.GetDir().z);
		shader->SetVec3("dirLight.ambient", dirLight.GetAmb());
		shader->SetVec3("dirLight.diffuse", dirLight.GetDiff());
		shader->SetVec3("dirLight.specular", dirLight.GetSpec());


		// Point Light 
		for (int i = 0; i < 4; ++i)
		{
			string index = to_string(i);
			string light = "pointLights[" + index + "]";

			shader->SetVec3(light + ".ambient", pointLights[i].GetAmb());
			shader->SetVec3(light + ".diffuse", pointLights[i].GetDiff());
			shader->SetVec3(light + ".specular", pointLights[i].GetSpec());
			
			shader->SetVec3(light + ".position", pointLights[i].GetPos());

			shader->SetFloat(light + ".constant", pointLights[i].GetConst());
			shader->SetFloat(light + ".linear", pointLights[i].GetLinear());
			shader->SetFloat(light + ".quadratic", pointLights[i].GetQuad());

		}
		
		spotLight.SetPos(camPos);
		spotLight.SetDir(camForward);
		
		shader->SetVec3("spotLight.position", camPos);
		shader->SetVec3("spotLight.direction", camForward);
		
		shader->SetVec3("spotLight.ambient", amb);
		shader->SetVec3("spotLight.diffuse", diff);
		shader->SetVec3("spotLight.specular", spec);

		shader->SetFloat("spotLight.constant", spotLight.GetConst());
		shader->SetFloat("spotLight.linear", spotLight.GetLinear());
		shader->SetFloat("spotLight.quadratic", spotLight.GetQuad());

		shader->SetFloat("spotLight.cutOff", spotLight.GetCutOff());
		shader->SetFloat("spotLight.outerCutOff", spotLight.GetOuterCutOff());

		//vec3 lightColor = vec3(1.0f);
		//lightColor.x = sin(currentFrame * 2.0);
		//lightColor.y = sin(currentFrame * 0.7);
		//lightColor.z = sin(currentFrame * 1.3);

		// Set material properties
		//shader->SetVec3("material.ambient", 1.0f, 0.5f, 0.31f);
		//shader->SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		//shader->SetVec3("material.specular", 0.5f, 0.5f, 0.5f);

		mat4 p = perspective(radians(camera->GetFov()), 800.0f / 600.0f, 0.1f, 100.0f);
		mat4 v = camera->MyLookAt();
		shader->SetMat4("projection", p);
		shader->SetMat4("view", v);

		mat4 m = mat4(1.0f);
		shader->SetMat4("model", m);

		// Bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		texture1->SetActive();

		glActiveTexture(GL_TEXTURE1);
		texture2->SetActive();
		
		glActiveTexture(GL_TEXTURE2);
		texture3->SetActive();

		//glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());

		// Render a cube
		glBindVertexArray(cubeVAO);
		for (int i = 0; i < 10; i++)
		{
			m = mat4(1.0f);
			m = translate(m, cubePositions[i]);
			shader->SetMat4("model", m);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lightShader->SetActive();
		lightShader->SetMat4("projection", p);
		lightShader->SetMat4("view", v);

		// Render a lamp
		glBindVertexArray(lightVAO);
		for (int i = 0; i < 4; ++i)
		{
			m = mat4(1.0f);
			m = translate(m, lightPositions[i]);
			m = scale(m, vec3(0.2f));
			lightShader->SetMat4("model", m);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		SDL_GL_SwapWindow(window);
	}

	shader->UnLoad();
	lightShader->UnLoad();
	texture1->Unload();
	texture2->Unload();
	texture3->Unload();

	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightVAO);

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	delete shader;
	delete lightShader;
	delete texture1;
	delete texture2;
	delete texture3;
	delete[] pointLights;

	return 0;
}