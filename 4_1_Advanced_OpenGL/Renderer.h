#pragma once

#include <windows.h>
#include <SDL.h>
#include <GL/glew.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <stb_image.h>
#include <map>

#include "Model.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

using namespace std;
using namespace glm;


class Renderer
{

public:
	enum class Shape
	{
		BOX = 0,
		CUBE,
	};

	Renderer();
	~Renderer();

	bool Initialize(float screen_width, float screen_height);

	void Draw();

	bool LoadModel(const string& path, bool flip);
	bool LoadShader();
	
	bool LoadPrimitive(const Shape& shape);


	vector<Vertex> LoadVertices(const string& vert_path);
	vector<unsigned int> LoadIndices(const string& index_path);

	void SetupLight();
	void UnLoad();
	void SortPosition();

	// Get functions
	int IsRunning() { return m_is_running; }
	SDL_Window* GetWindow() { return m_window; }
	SDL_GLContext GetContext() { return m_context; }
	Camera* GetCamera() { return m_camera; }

	//vector<float> GetVertices() { return m_light_vertices; }
	//vector<Vertex> m_light_vertices;
	//vector<unsigned int> m_light_indices;
	//unsigned int m_light_VBO;
	//unsigned int m_light_VAO;

private:

	SDL_Window* m_window;
	SDL_GLContext m_context;
	SDL_Event m_event;

	Camera* m_camera;
	
	Model* m_link;
	vector<Mesh> m_primitives;

	Shader* m_model_shader;
	Shader* m_light_shader;
	Shader* m_outline_shader;
	Shader* m_grass_shader;
	Shader* m_window_shader;
	Shader* m_screen_shader;

	Texture* m_grass_texture;
	Texture* m_window_texture;
	
	unsigned int m_quad_VAO;
	unsigned int m_quad_VBO;
	unsigned int m_FBO;
	GLint m_default_FBO;
	unsigned int m_framebuffer_texture;
	unsigned int m_RBO;

	int m_is_running = 1;
	int m_type = 0;

	float m_width;
	float m_height;

	map<float, vec3> m_sorted;

	// Lights colors
	vec3 m_amb = { 0.5f, 0.5f, 0.5f };
	vec3 m_diff = { 1.0f, 1.0f, 1.0f };
	vec3 m_spec = { 0.5f, 0.5f, 0.5f };
	vec3 m_dir = { -0.2f, -1.0f, -0.3f };

	// Positions
	vector<vec3> m_light_positions = {
		vec3(0.7f,  0.2f,  2.0f),
		vec3(2.3f, -3.3f, -4.0f),
		vec3(-4.0f,  2.0f, -12.0f),
		vec3(0.0f,  0.0f, -3.0f)
	};

	vector<vec3> m_vegetations = {
		vec3(-1.5f, 0.0f, -0.48f),
		vec3(1.5f, 0.0f, 0.51f),
		vec3(0.0f, 0.0f, 0.7f),
		vec3(-0.3f, 0.0f, -2.3f),
		vec3(0.5f, 0.0f, -0.6f)
	};

	vector<vec3> m_windows = {
		vec3(2.0f, 0.0, -2.5f),
		vec3(1.8f, 0.0, -3.0f)
	};

	float m_quad_vertices[24] = {
		// positions   // texCoords
		// Left triangle
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		// Right triangle
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
};