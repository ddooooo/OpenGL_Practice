#include "Renderer.h"
#include <fstream>
#include <sstream>
#include <random>
#include <math.h>

Renderer::Renderer() : m_is_running(1), m_width(0), m_height(0)
{
	m_window = nullptr;
	m_context = 0;
}

Renderer::~Renderer() {}

bool Renderer::Initialize(const float screen_width, const float screen_height)
{
	cout << "Initialize" << endl;
	m_width = screen_width;
	m_height = screen_height;

	if (!InitSetup())
	{
		printf("Failed to init setup \n");
		return false;
	}

	if (!InitPrimitive())
	{
		return false;
	}

	if (!InitModel())
	{
		return false;
	}

	if (!InitShader())
	{
		return false;
	}

	if (!InitShadowFBO())
	{
		return false;
	}

	m_camera = unique_ptr<Camera>(new Camera());
	
	return true;
}

bool Renderer::InitSetup()
{
	cout << "InitSetup" << endl;
	SDL_Init(SDL_INIT_VIDEO);

	// Set core OpenGL profile
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

	m_window = SDL_CreateWindow("Shadow Mapping", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_OPENGL);

	if (!m_window)
	{
		printf("Failed to load window: %s \n", SDL_GetError());
		return false;
	}

	m_context = SDL_GL_CreateContext(m_window);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		printf("Failed to initialize GLEW. \n");
		return false;
	}

	glGetError();

	// Enable depth buffer
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	printf("%s \n", glGetString(GL_VERSION));

	return true;
}

bool Renderer::InitPrimitive()
{
	cout << "InitPrimitive" << endl;
	m_primitives[Shape::FLOOR] = unique_ptr<Primitive>(new Primitive(Shape::FLOOR));
	m_primitives[Shape::SQUARE] = unique_ptr<Primitive>(new Primitive(Shape::SQUARE));
	m_primitives[Shape::CUBE] = unique_ptr<Primitive>(new Primitive(Shape::CUBE));
	for (auto& it : m_primitives)
	{
		if (!it.second->LoadPrimitive())
		{
			cerr << "Faield to load Primitive: " << it.first << endl;
			return false;
		}
	}

	return true;
}

bool Renderer::InitModel()
{
	cout << "InitModel" << endl;
	m_models["planet"] = unique_ptr<Model>(new Model("Models/planet/planet.obj"));

	for (auto& it : m_models)
	{
		if (!it.second->LoadModel())
		{
			cout << "Failed to load model " << it.first << endl;
			return false;
		}
	}

	return true;
}

bool Renderer::InitShader()
{
	cout << "InitShader" << endl;
	m_shaders["shadowMap"] = unique_ptr<Shader>(new Shader("Shaders/Point_Shadow_Map.vert", "Shaders/Point_Shadow_Map.frag", 
															"Shaders/Point_Shadow_Map.geom"));
	m_shaders["shadow"] = unique_ptr<Shader>(new Shader("Shaders/Point_Shadow.vert", "Shaders/Point_Shadow.frag"));
	m_shaders["light"] = unique_ptr<Shader>(new Shader("Shaders/Light.vert", "Shaders/Light.frag"));
	for (auto& it : m_shaders)
	{
		if (!it.second->LoadShaderFile())
		{
			cout << "Failed to load shader file " << it.first << endl;
			return false;
		}
	}

	m_shaders["shadow"]->SetActive();
	m_shaders["shadow"]->SetInt("shadowMap", 0);

	return true;
}

bool Renderer::InitShadowFBO()
{
	cout << "InitShadowFBO" << endl;
	m_shadow_FBO = unique_ptr<ShadowCubemapFBO>(new ShadowCubemapFBO(SHADOW_WIDTH, SHADOW_HEIGHT));
	if (!m_shadow_FBO->Init())
	{
		return false;
	}

	return true;
}

void Renderer::Run()
{
	cout << "Run" << endl;
	while (m_is_running)
	{
		Render();
	}
}

void Renderer::Render()
{
	//cout << "Render " << endl;
	// Get current frame to calculate camera delta time 
	float current_frame = (float)SDL_GetTicks() * 0.001f;
	float last_frame = m_camera->GetLastFrame();
	m_camera->SetDeltaTime(current_frame - last_frame);
	m_camera->SetLastFrame(current_frame);

	m_camera->ProcessInput();

	HandleEvent();

	m_light_pos.x = static_cast<float>(sin(SDL_GetTicks() * 0.001f) * 3.0);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	ShadowPass();

	LightPass();

	SDL_GL_SwapWindow(m_window);
}

void Renderer::HandleEvent()
{
	//cout << "Handle Event" << endl;
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			m_is_running = 0;
		}

		if (event.type == SDL_MOUSEBUTTONUP)
		{
			m_camera->ProcessMouseUp(event, m_window,
				static_cast<int>(m_width / 2), static_cast<int>(m_height / 2));
		}

		if (event.type == SDL_MOUSEMOTION)
		{
			m_camera->ProcessMouseDown(event);
		}
	}
}

void Renderer::ShadowPass()
{
	//cout << "Render shadow map" << endl;
	float near_plane = 1.0f;
	float far_plane = 25.0f;
	mat4 P_light = perspective(radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT,
								near_plane, far_plane);
	vector<mat4> matrices_light;

	// Right Cubemap
	mat4 V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	matrices_light.push_back(P_light * V_light);
	// Left Cubemap
	V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	matrices_light.push_back(P_light * V_light);
	// Up Cubemap
	V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	matrices_light.push_back(P_light * V_light);
	// Down Cubemap
	V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	matrices_light.push_back(P_light * V_light);
	// Front Cubemap
	V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	matrices_light.push_back(P_light * V_light);
	// Back Cubemap
	V_light = lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	matrices_light.push_back(P_light * V_light);

	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	//matrices_light.push_back(P_light * glm::lookAt(m_light_pos, m_light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));


	// Render scene to shadow cubemap
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_shadow_FBO->BindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);

	m_shaders["shadowMap"]->SetActive();

	for (unsigned int i = 0; i < 6; ++i)
	{
		m_shaders["shadowMap"]->SetMat4("shadowMatrices[" + to_string(i) + "]", matrices_light[i]);
	}
	m_shaders["shadowMap"]->SetVec3("lightPos", m_light_pos);
	m_shaders["shadowMap"]->SetFloat("far_plane", far_plane);

	RenderScene(*m_shaders["shadowMap"]);
}

void Renderer::LightPass()
{
	//cout << "Render color map" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 P = perspective(radians(90.0f), m_width / m_height, 0.1f, 100.0f);
	mat4 V = m_camera->MyLookAt();
	mat4 M = mat4(1.0f);
	M = translate(M, m_light_pos);
	M = scale(M, vec3(0.2f));

	m_shaders["light"]->SetActive();
	m_shaders["light"]->SetMat4("projection", P);
	m_shaders["light"]->SetMat4("view", V);
	m_shaders["light"]->SetMat4("model", M);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	m_shaders["shadow"]->SetActive();
	m_shaders["shadow"]->SetMat4("projection", P);
	m_shaders["shadow"]->SetMat4("view", V);

	m_shaders["shadow"]->SetVec3("lightPos", m_light_pos);
	m_shaders["shadow"]->SetVec3("viewPos", m_camera->GetPos());
	m_shaders["shadow"]->SetFloat("far_plane", 25.0f);

	m_shadow_FBO->BindFrameTexture();
	RenderScene(*m_shaders["shadow"]);
}

void Renderer::RenderScene(Shader& shader)
{
	//cout << "RenderScene" << endl;
	mat4 M = mat4(1.0f);
	shader.SetMat4("model", M);
	m_primitives[Shape::FLOOR]->SetActive();
	m_primitives[Shape::FLOOR]->Draw();

	M = translate(M, vec3(0.0, 10.0f, 0.0));
	shader.SetMat4("model", M);
	shader.SetInt("reverse_normals", 1);
	m_primitives[Shape::FLOOR]->SetActive();
	m_primitives[Shape::FLOOR]->Draw();

	M = mat4(1.0f);
	M = translate(M, vec3(0.0f, 1.5f, 0.0f));
	M = scale(M, vec3(0.5f));
	shader.SetMat4("model", M);
	shader.SetInt("reverse_normals", 0);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	M = mat4(1.0f);
	M = translate(M, vec3(2.0f, 0.5f, 1.0f));
	M = scale(M, vec3(0.5f));
	shader.SetMat4("model", M);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	M = mat4(1.0f);
	M = translate(M, vec3(-1.0f, 0.0f, 2.0f));
	M = rotate(M, radians(60.0f), normalize(vec3(1.0, 0.0, 1.0)));
	M = scale(M, vec3(0.25f));
	shader.SetMat4("model", M);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	M = mat4(1.0f);
	M = translate(M, vec3(0.0, 7.0f, 0.0));
	M = scale(M, vec3(0.6f));
	shader.SetMat4("model", M);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();
}

void Renderer::UnLoad()
{
	for (auto& it : m_shaders)
	{
		it.second->UnLoad();
	}

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}