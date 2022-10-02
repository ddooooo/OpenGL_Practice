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
#include "Animator.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Primitive.h"
#include "ShadowCubemapFBO.h"
#include "PointLight.h"
#include "Config.h"

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
	void InitAnimation();
	bool InitShader();
	bool InitTexture();
	void InitLight();

	void Run();
	void Render();

	void HandleEvent();

	void RenderScene();

	void UnLoad();
	
	void AdjustPrecision(mat4& transform);

private:
	SDL_Window* m_window;
	SDL_GLContext m_context;

	unordered_map<ModelName, shared_ptr<Model>> m_models;
	unordered_map<Shape, unique_ptr<Primitive>> m_primitives;
	unordered_map<string, unique_ptr<Shader>> m_shaders;
	unordered_map<string, unique_ptr<Texture>> m_brick_textures;
	
	unique_ptr<PointLight> m_point_light;
	unique_ptr<Camera> m_camera;

	unique_ptr<Animator> m_animator;

	int m_is_running;
	int m_display_weight;
	float m_ticks;

	long long m_start_time;

	float m_width;
	float m_height;
};