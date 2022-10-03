#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <random>
#include <math.h>

Renderer::Renderer() : m_is_running(1), m_width(0), m_height(0), 
					   m_display_weight(-1), m_ticks(0), m_start_time(0)
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

	InitSetup();

	InitTerrain();

	InitPrimitive();

	InitShader();

	m_camera = unique_ptr<Camera>(new Camera());
	
	m_start_time = SDL_GetTicks64();
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

	m_window = SDL_CreateWindow("Terrain", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
							static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_OPENGL);

	if (!m_window)
	{
		printf("Failed to load window: %s \n", SDL_GetError());
		assert(0);
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
	m_primitives[Shape::CUBE] = make_unique<Primitive>(Shape::CUBE);
	//m_primitives[Shape::SQUARE] = make_unique<Primitive>(Shape::SQUARE);
	for (auto& it : m_primitives)
	{
		if (!it.second || !it.second->LoadPrimitive())
		{
			cerr << "Faield to load Primitive: " << it.first << endl;
			assert(0);
			return false;
		}
		else
		{
			cout << "Successfully load primitive: " << it.first << endl;
		}
	}
	return true;
}

bool Renderer::InitModel()
{
	cout << "InitModel" << endl;
	
	int index = static_cast<int>(ModelName::LINK);
	m_models[ModelName::LINK] = unique_ptr<Model>(new Model(enumTostring[index]));
	
	for (auto& it : m_models)
	{
		if (!(it.second) || !(it.second->LoadModel()))
		{
			cout << "Failed to load model " << (int)it.first << endl;
			assert(0);
			return false;
		}
		else 
		{
			cout << it.second->GetPath() << endl;
		}
	}
	return true;
}

void Renderer::InitAnimation()
{
	cout << "Init Animation" << endl;
	
	m_animator = make_unique<Animator>(m_models.at(ModelName::LINK));
	
	assert(m_animator);
}

bool Renderer::InitShader()
{
	cout << "InitShader" << endl;
	
	m_shaders["terrain"] = unique_ptr<Shader>(new Shader("Shaders/Terrain.vert", "Shaders/Terrain.frag"));
	m_shaders["light"] = unique_ptr<Shader>(new Shader("Shaders/Light.vert", "Shaders/Light.frag"));

	for (auto& it : m_shaders)
	{
		if ((!it.second) || !(it.second->LoadShaderFile()))
		{
			cout << "Failed to load shader file " << it.first << endl;
			assert(0);
			return false;
		}
		else
		{
			cout << "Successfully load shader: " << it.first << endl;
		}
	}

	return true;
}

bool Renderer::InitTexture()
{
	cout << "Init Texture" << endl;
	m_brick_textures["diffuse"] = unique_ptr<Texture>(new Texture("Textures/Brick/brick.jpg"));
	m_brick_textures["normal"] = unique_ptr<Texture>(new Texture("Textures/Brick/brick_normal.jpg"));
	m_brick_textures["height"] = unique_ptr<Texture>(new Texture("Textures/Brick/brick_height.jpg"));

	for (auto& it : m_brick_textures)
	{
		if (!(it.second) || !(it.second->LoadTexture()))
		{
			cerr << "Failed to load texture: " << it.first << endl;
			assert(0);
			return false;
		}
		else
		{
			cout << "Successfully loaded texture: " << it.first << endl;
		}
	}

	return true;
}

void Renderer::InitLight()
{
	cout << "Init light" << endl;
	vec3 pos = { 0.0f, 1.0f, 1.0f };
	
	vec3 amb = { 1.0f, 1.0f, 1.0f };
	vec3 diff = { 1.0f, 1.0f, 1.0f };
	vec3 spec = { 1.0f, 1.0f, 1.0f };
	vec3 dir = { 0.0f, -0.5f, -0.5f };

	float c = 0.0f;
	float l = 0.0f;
	float q = 0.0f;


	m_point_light = make_unique<PointLight>(amb, diff, spec, pos, c, l, q);

	assert(m_point_light);
}

void Renderer::InitTerrain()
{

	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/Terrain.png", &width, &height, &nrChannels, 0);
	assert(data);

	vector<VertexLayout> vertices;
	float y_scale = 64.0f / 256.0f;
	float y_shift = 16.0f;
	int byte_per_pixel = nrChannels;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			VertexLayout vertex;
			
			int offset = (j + width * i) * byte_per_pixel;
			const unsigned char* pixel = data + offset;
			const unsigned char y = pixel[0];

			vertex.position.x = static_cast<float>( (-height / 2.0f) + i );
			vertex.position.y = ((int)y * y_scale - y_shift);
			//cout << "Position y: " << vertex.position.y << endl;
			vertex.position.z = static_cast<float>( (-width / 2.0f) + j );
		
			vertices.push_back(vertex);
		}
	}

	vector<unsigned int> indices;
	for (unsigned int i = 0; i < height - 1; ++i)
	{
		for (unsigned int j = 0; j < width; ++j)
		{
			for (unsigned int k = 0; k < 2; ++k)
			{
				indices.push_back(j + width * (i + k));
			}
		}
	}

	m_terrain = make_unique<TerrainMesh>("Terrain", width, height, vertices, indices);

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
	float current_frame = (float)SDL_GetTicks64() * 0.001f;
	float last_frame = m_camera->GetLastFrame();
	m_camera->SetDeltaTime(current_frame - last_frame);
	m_camera->SetLastFrame(current_frame);

	m_camera->ProcessInput();

	HandleEvent();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderScene();

	SDL_GL_SwapWindow(m_window);
}

void Renderer::HandleEvent()
{
	//cout << "Handle Event" << endl;
	SDL_Event event;

	const Uint8* keyState = SDL_GetKeyboardState(NULL);
	if (keyState[SDL_SCANCODE_SPACE] && m_ticks > 2)
	{
		m_display_weight += 1;
		m_display_weight = m_display_weight % m_models.at(ModelName::LINK)->GetNumBones();
		cout << "Spacebar pressed " << m_display_weight << " " << m_models.at(ModelName::LINK)->GetNumBones() << endl;
		m_ticks = 0;
	}

	m_ticks += m_camera->GetDeltaTime();

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

void Renderer::RenderScene()
{
	assert(m_terrain && m_shaders["terrain"]);

	long long current_time = SDL_GetTicks64();
	float dt = static_cast<float>(current_time - m_start_time);
	m_start_time = current_time;
	
	//cout << "Current time: " << current_time << endl;
	//cout << "dt: " << dt << endl;

	mat4 P = perspective(radians(m_camera->GetFov()), m_width / m_height, 0.1f, 100.0f);
	mat4 V = m_camera->MyLookAt();
	mat4 M = mat4(1.0f);
	M = scale(M, vec3(0.01f));
	m_shaders["terrain"]->SetActive();
	m_shaders["terrain"]->SetMat4("projection", P);
	m_shaders["terrain"]->SetMat4("view", V);
	m_shaders["terrain"]->SetMat4("model", M);
	m_terrain->Draw();

	M = mat4(1.0f);
	M = translate(M, vec3(0.0f, 10.0f, 0.0f));
	M = scale(M, vec3(0.1f));
	m_shaders["light"]->SetActive();
	m_shaders["light"]->SetMat4("projection", P);
	m_shaders["light"]->SetMat4("view", V);
	m_shaders["light"]->SetMat4("model", M);
	m_primitives[Shape::CUBE]->Draw();
}

void Renderer::AdjustPrecision(mat4& transform)
{
	for (int i = 0; i < transform.length(); ++i)
	{
		for (int j = 0; j < transform.length(); ++j)
		{
			if (abs(transform[i][j]) < 1.0e-6)
			{
				transform[i][j] = 0.0f;
			}
		}
	}
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