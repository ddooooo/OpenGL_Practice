#include "Renderer.h"

Renderer::Renderer() : 
	m_is_running(true), m_ticks(0), m_start_time(0), 
	m_sdl_window(make_unique<SDL_GL_Window>()), m_camera(unique_ptr<Camera>()),
	m_model(make_unique<Mesh>()), m_shader(make_unique<Shader>())
{}

Renderer::~Renderer() {}

bool Renderer::init()
{
	cout << "Initialize Renderer" << endl;

	int screen_width = 1024;
	int screen_height = 720;

	float aspect = (float)screen_width / screen_height;

	m_sdl_window->init(screen_width, screen_height, "Refractoring & ImGui");

	if (!m_model->importMeshFromFile("Models/Rectangle.txt")) 
		return false;

	m_shader->loadShaderFile("Shaders/Basic.vert", "Shaders/Basic.frag");
	
	m_camera = make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f), aspect, 45.0f, -90.0f, 0.0f );

	m_start_time = SDL_GetTicks64();

	return true;
}


void Renderer::run()
{
	cout << "Run" << endl;

	while (m_is_running)
	{
		render();
	}
}

void Renderer::render()
{
	float current_frame = (float)SDL_GetTicks() * 0.001f;
	float last_frame = m_camera->getLastFrame();
	m_camera->setDeltaTime(current_frame - last_frame);
	m_camera->setLastFrame(current_frame);
	m_camera->processInput();

	handleInput();

	renderImGui();

	m_sdl_window->clearWindow();
	renderScene();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	m_sdl_window->swapWindow();
}

void Renderer::handleInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			m_is_running = 0;

		if (event.type == SDL_MOUSEBUTTONUP)
			m_camera->processMouseUp(event, m_sdl_window.get());

		if (event.type == SDL_MOUSEMOTION)
			m_camera->processMouseDown(event);
	}
}

void Renderer::renderImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!");                          
	ImGui::Text("This is some useful text.");              

	if (ImGui::Button("Button"))
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	float frameRate = ImGui::GetIO().Framerate;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / frameRate, frameRate);
	ImGui::End();

	// Rendering
	ImGui::Render();
}

void Renderer::renderScene()
{
	mat4 P = glm::perspective(glm::radians(45.0f), 1024.0f / 720.0f, 0.1f, 100.0f);
	mat4 V = m_camera->camera2pixel();
	mat4 M = mat4(1.0f);

	m_shader->load();
	m_shader->setPVM(P, V, M);
	m_model->draw();
}

void Renderer::end()
{
	m_shader.release();
	m_model.release();
	m_sdl_window->unload();
}