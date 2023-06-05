#pragma once

#include <glm/glm.hpp>

#include "SDL_GL_Window.h"
#include "Camera.h"
#include "Mesh.h"

using namespace std;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool init();
	void run();
	void render();

	void handleInput();
	void renderScene();
	void renderImGui();

	void end();

private:
	unique_ptr<SDL_GL_Window> m_sdl_window;
	unique_ptr<Mesh> m_model;
	unique_ptr<Shader> m_shader;
	unique_ptr<Camera> m_camera;

	bool m_is_running;
	float m_ticks;
	long long m_start_time;
};