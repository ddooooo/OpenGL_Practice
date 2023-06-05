#include <cassert>
#include "SDL_GL_Window.h"

SDL_GL_Window::SDL_GL_Window() :
	m_window(nullptr), m_context(0), 
	m_width(0), m_height(0), m_title("")
{}

void SDL_GL_Window::init(const int width, const int height, string title)
{
	cout << "init from sdl gl window" << endl;
	m_width = width;
	m_height = height;
	m_title = title;

	SDL_Init(SDL_INIT_VIDEO);
	
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Set version of SDL 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set a buffer with 8 bits for each channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	// Set double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Hardware Acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width, m_height, SDL_WINDOW_OPENGL);

	if (!m_window)
	{
		printf("Failed to load window: %s \n", SDL_GetError());
		assert(0);
	}

	m_context = SDL_GL_CreateContext(m_window);
	SDL_GL_MakeCurrent(m_window, m_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	cout << "Create imgui context" << endl;
	IMGUI_CHECKVERSION();
	ImGui::CreateContext(); 
	ImGuiIO& m_io = ImGui::GetIO(); (void)m_io;
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	m_io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	const char* glsl_version = "#version 330";

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW. \n");
		assert(0);
	}

	glGetError();

	glEnable(GL_DEPTH_TEST);

	printf("%s \n", glGetString(GL_VERSION));
}

void SDL_GL_Window::clearWindow()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDL_GL_Window::swapWindow()
{
	SDL_GL_SwapWindow(m_window);
}

void SDL_GL_Window::unload()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}
