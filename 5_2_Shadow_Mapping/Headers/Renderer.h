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
#include <unordered_map>

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
		SQUARE = 0,
		CUBE,
		FLOOR,
	};

	Renderer();
	~Renderer();

	bool InitSetup();
	bool Initialize(const float screen_width, const float screen_height);


	void DrawModels();
	void Draw();
	void RenderScene(Shader& shader);

	unique_ptr<Model> LoadModel(const string& path, int amount, bool flip);
	unique_ptr<Mesh> LoadPrimitive(const Shape& shape);
	unique_ptr<Mesh> LoadSkybox(const string& v_path, const string& i_path);
	unique_ptr<Shader> LoadShaders(const string& vert, const string& frag, const string& geom = "");
	unique_ptr<Texture> LoadSkyboxTex(const vector<string>& faces);

	vector<Vertex> LoadVertices(const string& vert_path);
	vector<unsigned int> LoadIndices(const string& index_path);

	bool SetupShadow();
	//void SetupMSAA();
	//void SetupUBO();
	//void SetupLight();
	void UnLoad();

	mat4 MyOrtho(float l, float r, float b, float t, float f, float n);

	int IsRunning() { return m_is_running; }

private:

	SDL_Window* m_window;
	SDL_GLContext m_context;

	unordered_map<string, unique_ptr<Model>> m_models;
	unordered_map<Shape, unique_ptr<Mesh>> m_primitives;
	unordered_map<string, unique_ptr<Shader>> m_shaders;
	
	unique_ptr<Texture> m_texture;
	
	unique_ptr<Camera> m_camera;

	int m_is_running;
	int m_type;

	float m_width;
	float m_height;

	unsigned int m_shadow_FBO;
	unsigned int m_shadow_map;

	unsigned int m_frame_VAO;
	unsigned int m_frame_VBO;

	float vertices[24] = {
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


};