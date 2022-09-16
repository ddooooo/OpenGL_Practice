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
#include "Primitive.h"
#include "ShadowCubemapFBO.h"

using namespace std;
using namespace glm;
typedef Primitive::Shape Shape;

class Renderer
{

public:
	Renderer();
	~Renderer();

	bool Initialize(const float screen_width, const float screen_height);
	bool InitSetup();
	bool InitPrimitive();
	bool InitModel();
	bool InitShader();
	bool InitShadowFBO();


	void Run();
	void Render();

	void HandleEvent();

	void ShadowPass();
	void LightPass();
	void RenderScene(Shader& shader);

	void UnLoad();
	

private:
	SDL_Window* m_window;
	SDL_GLContext m_context;

	unordered_map<string, unique_ptr<Model>> m_models;
	unordered_map<Primitive::Shape, unique_ptr<Primitive>> m_primitives;
	unordered_map<string, unique_ptr<Shader>> m_shaders;
	
	unique_ptr<ShadowCubemapFBO> m_shadow_FBO;
	unique_ptr<Texture> m_texture;
	unique_ptr<Camera> m_camera;

	int m_is_running;

	float m_width;
	float m_height;

	vec3 m_light_pos = { 0.0f, 3.0f, 0.0f };

	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

};