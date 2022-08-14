#pragma once

#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

using namespace std;
using namespace glm;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Initialize(float screen_width, float screen_height);

	void Draw();

	bool LoadModel();
	bool LoadShader();
	bool LoadVertices();
	bool LoadIndices();

	void UnLoad();

	// Get functions
	int IsRunning() { return m_is_running; }
	SDL_Window* GetWindow() { return m_window; }
	SDL_GLContext GetContext() { return m_context; }
	Camera* GetCamera() { return m_camera; }


	Shader* GetShader() { return m_shader; }
	Shader* GetLightShader() { return m_light_shader; }
	Mesh* GetMesh() { return m_light_mesh; }

	//vector<float> GetVertices() { return m_light_vertices; }
	vector<Vertex> m_light_vertices;
	vector<unsigned int> m_light_indices;
	unsigned int m_light_VBO;
	unsigned int m_light_VAO;

private:

	SDL_Window* m_window;
	SDL_GLContext m_context;
	SDL_Event m_event;

	int m_is_running = 1;

	float m_width;
	float m_height;

	Model* m_model;
	Shader* m_model_shader;

	Camera* m_camera;

	Shader* m_shader;
	Shader* m_light_shader;

	Mesh* m_light_mesh;

	vector<vec3> m_light_positions = {
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f)
	};

};