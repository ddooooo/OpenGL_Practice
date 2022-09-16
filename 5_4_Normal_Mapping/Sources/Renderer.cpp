#include "..\Headers\Renderer.h"
#include <fstream>
#include <sstream>
#include <random>
#include <math.h>

Renderer::Renderer() : m_is_running(1), m_type(0), m_width(0), m_height(0),
					   m_shadow_FBO(0), m_shadow_map(0), m_frame_VAO(0), m_frame_VBO(0)
{
	m_window = nullptr;
	m_context = 0;
}

Renderer::~Renderer()
{
}

bool Renderer::InitSetup()
{
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
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	//SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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
	glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);

	printf("%s \n", glGetString(GL_VERSION));

	return true;
}

bool Renderer::Initialize(const float screen_width, const float screen_height)
{
	m_width = screen_width;
	m_height = screen_height;

	if (!InitSetup())
	{
		printf("Failed to init setup \n");
		return false;
	}

	m_primitives[Shape::FLOOR] = LoadPrimitive(Shape::FLOOR);
 	m_primitives[Shape::SQUARE] = LoadPrimitive(Shape::SQUARE);
	m_primitives[Shape::CUBE] = LoadPrimitive(Shape::CUBE);
	for (auto& it : m_models)
	{
		if (it.second == nullptr)
		{
			cout << "Faield to load planet or rock" << endl;
			return false;
		}
	}

	m_shaders["shadowMap"] = LoadShaders("Shaders/5_2_ShadowMap.vert", "Shaders/5_2_ShadowMap.frag");
	m_shaders["shadow"] = LoadShaders("Shaders/5_2_Shadow.vert", "Shaders/5_2_Shadow.frag");
	m_shaders["light"] = LoadShaders("Shaders/5_2_Light.vert", "Shaders/5_2_Light.frag");

	for (auto& it : m_shaders)
	{
		if (it.second == nullptr)
		{
			cout << "Failed to load one of the shaders" << endl;
			return false;
		}
	}

	m_shaders["shadow"]->SetActive();
	m_shaders["shadow"]->SetInt("shadowMap", 0);

	m_camera = unique_ptr<Camera>(new Camera());

	if (!SetupShadow())
	{
		return false;
	}


	return true;
}

void Renderer::Draw()
{

	// Get current frame for camera delta time 
	float current_frame = (float)SDL_GetTicks() * 0.001f;
	float last_frame = m_camera->GetLastFrame();
	m_camera->SetDeltaTime(current_frame - last_frame);
	m_camera->SetLastFrame(current_frame);

	m_camera->ProcessInput();

	SDL_Event m_event;

	while (SDL_PollEvent(&m_event))
	{
		if (m_event.type == SDL_QUIT)
		{
			m_is_running = 0;
		}

		if (m_event.type == SDL_MOUSEBUTTONUP)
		{
			m_camera->ProcessMouseUp(m_event, m_window, 
				static_cast<int>(m_width/2), static_cast<int>(m_height/2));
		}

		if (m_event.type == SDL_MOUSEMOTION)
		{
			m_camera->ProcessMouseDown(m_event);		
		}
	}

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_shadow_FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	vec3 light_pos = { -2.0f, 4.0f, -1.0f };
	mat4 ortho_proj, light_view;
	ortho_proj = ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);
	light_view = lookAt(light_pos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	mat4 light_proj = ortho_proj * light_view;

	m_shaders["shadowMap"]->SetActive();
	m_shaders["shadowMap"]->SetMat4("lightProjection", light_proj);
	glCullFace(GL_FRONT);
	RenderScene(*m_shaders["shadowMap"]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glCullFace(GL_BACK);
	glViewport(0, 0, static_cast<GLsizei>(m_width), static_cast<GLsizei>(m_height));
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	mat4 p = perspective(radians(m_camera->GetFov()), m_width / m_height, 0.1f, 100.0f);
	mat4 v = m_camera->MyLookAt();
	mat4 m = mat4(1.0f);
	m = translate(m, light_pos);
	m = scale(m, vec3(0.2f));

	m_shaders["light"]->SetActive();
	m_shaders["light"]->SetMat4("projection", p);
	m_shaders["light"]->SetMat4("view", v);
	m_shaders["light"]->SetMat4("model", m);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	m_shaders["shadow"]->SetActive();
	m_shaders["shadow"]->SetMat4("projection", p);
	m_shaders["shadow"]->SetMat4("view", v);

	m_shaders["shadow"]->SetMat4("lightMatrix", light_proj);
	m_shaders["shadow"]->SetVec3("lightPos", light_pos);
	m_shaders["shadow"]->SetVec3("viewPos", m_camera->GetPos());


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_shadow_map);

	RenderScene(*m_shaders["shadow"]);

	//glDisable(GL_DEPTH_TEST);
	//m_shaders["frame"]->SetActive();
	//glBindVertexArray(m_frame_VAO);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_shadow_map);
	//glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(m_window);
}

void Renderer::RenderScene(Shader& shader)
{
	mat4 m = mat4(1.0f);
	shader.SetMat4("model", m);
	m_primitives[Shape::FLOOR]->SetActive();
	m_primitives[Shape::FLOOR]->Draw();

	m = mat4(1.0f);
	m = translate(m, vec3(0.0f, 1.5f, 0.0f));
	m = scale(m, vec3(0.5f));
	shader.SetMat4("model", m);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	m = mat4(1.0f);
	m = translate(m, vec3(2.0f, 0.5f, 1.0f));
	m = scale(m, vec3(0.5f));
	shader.SetMat4("model", m);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();

	m = mat4(1.0f);
	m = translate(m, vec3(-1.0f, 0.0f, 2.0f));
	m = rotate(m, radians(60.0f), normalize(vec3(1.0, 0.0, 1.0)));
	m = scale(m, vec3(0.25f));
	shader.SetMat4("model", m);
	m_primitives[Shape::CUBE]->SetActive();
	m_primitives[Shape::CUBE]->Draw();
}

unique_ptr<Model> Renderer::LoadModel(const string& path, int amount, bool flip)
{
	//m_link = new Model();
	unique_ptr<Model> model;
	cout << "Load Model from " << path.c_str() << endl;
	if (amount > 0)
	{
		// Set up model matrices for rocks

		vector<mat4> matrices = vector<mat4>(amount);

		srand(static_cast<unsigned int>(SDL_GetTicks()));
		float r = 30.0f;

		for (int i = 0; i < amount; ++i)
		{
			mat4 m = mat4(1.0f);

			random_device rd;  // Will be used to obtain a seed for the random number engine
			mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
			float pi = static_cast<float>(M_PI);
			uniform_real_distribution<float> angle(0.0f, 360.0f);
			float a = angle(gen);
			//cout << "Angle: " << a << endl;
			float radian = a * (pi/180.0f);
			
			uniform_real_distribution<float> offset(-5.0f, 5.0f);


			float x = sin(radian) * r + offset(gen);
			float y = offset(gen) * 0.4f;
			float z = cos(radian) * r + offset(gen);
			
			//cout << "random location: " << x << " " << y  << " " << z << endl;
			m = translate(m, vec3(x, y, z));

			uniform_real_distribution<float> sc(0.05f, 0.1f);
			//float s = static_cast<float>((rand() % 20) / 100.0 + 0.05);
			m = scale(m, vec3(sc(gen)));

			uniform_real_distribution<float> rot(0.0f, 360.0f);
			m = rotate(m, rot(gen), vec3(0.4f, 0.6f, 0.8f));

			matrices[i] = m;
		}
		model = unique_ptr<Model>(new Model(matrices));
	}
	else
	{
		model = unique_ptr<Model>(new Model());
	}

	stbi_set_flip_vertically_on_load(flip);
	if (!model->LoadModel(path))
	{
		printf("Renderer failed to load model %s \n", path.c_str());
		return nullptr;
	}

	return model;
}

unique_ptr<Mesh> Renderer::LoadPrimitive(const Shape& shape)
{
	//cout << "Load Primitive!" << endl;

	string v_path = "";
	string i_path = "";
	
	switch (shape)
	{
		case(Shape::SQUARE):
			v_path = "Vertices/Plane/Plane_Vertices_List.txt";
			i_path = "Vertices/Plane/Plane_Indices_List.txt";
			break;

		case(Shape::CUBE):
			v_path = "Vertices/Cube/Cube_Vertices_List.txt";
			i_path = "Vertices/Cube/Cube_Indices_List.txt";
			break;

		case(Shape::FLOOR):
			v_path = "Vertices/Plane/Floor_Vertices_List.txt";
			i_path = "Vertices/Plane/Plane_Indices_List.txt";
			break;

		default:
			cout << "Invalide Shape!" << endl;
			return nullptr;
	}

	vector<Vertex> vertices = LoadVertices(v_path);
	vector<unsigned int> indices = LoadIndices(i_path);

	if (vertices.empty())
	{
		printf("Renderer Failed to load vertices \n");
		return nullptr;
	}

	if (indices.empty())
	{
		printf("Renderer Failed to load indices \n");
		return nullptr;
	}

	unique_ptr<Mesh> primitive(new Mesh(vertices, indices));
	primitive->SetupMesh();

	return primitive;
}

unique_ptr<Shader> Renderer::LoadShaders(const string& vert, const string& frag, const string& geom)
{
	cout << "Load Shader!" << endl;

	unique_ptr<Shader> temp(new Shader());
	if (!temp->LoadShaderFile(vert, frag, geom))
	{
		printf("Renderer Failed to load shader \n");
		return nullptr;
	}

	return temp;
}

vector<Vertex> Renderer::LoadVertices(const string& vert_path)
{
	vector<Vertex> vertices;

	//cout << "Load Vertices!" << endl;
	
	ifstream vertex_file(vert_path);
	
	if (!vertex_file.is_open())
	{
		cout << "Renderer Failed to open the vertex file " << vert_path << endl;
		return {};
	}

	string line;
	int index_tex = 0;
	int index_nor = 0;
	while (getline(vertex_file, line))
	{
		stringstream ss(line);
		if (line[0] == '#')
		{
			//cout << "# ignore" << endl;
			continue;
		}
		else if (line[0] == 'v')
		{
			//cout << "Vertex load" << endl;
			m_type = 0;
			continue;
		}
		else if (line[0] == 'n')
		{
			//cout << line << endl;
			m_type = 1;
			continue;
		}
		else if (line[0] == 't')
		{
			//cout << "Texture coordinate load" << endl;
			m_type = 2;
			continue;
		}

		if (m_type == 0)
		{
			Vertex vertex;
			string vertex_x;
			string vertex_y;
			string vertex_z;

			getline(ss, vertex_x, ',');
			getline(ss, vertex_y, ',');
			getline(ss, vertex_z, ',');


			//vertex.position = position;
			vertex.position.x = stof(vertex_x);
			vertex.position.y = stof(vertex_y);
			vertex.position.z = stof(vertex_z);
			vertices.push_back(vertex);
		}
		else if (m_type == 1)
		{
			string normal_x;
			string normal_y;
			string normal_z;

			getline(ss, normal_x, ',');
			getline(ss, normal_y, ',');
			getline(ss, normal_z);

			Vertex& vertex = vertices[index_nor];

			vertex.normal.x = stof(normal_x);
			vertex.normal.y = stof(normal_y);
			vertex.normal.z = stof(normal_z);
			
			++index_nor;
		}
		else if (m_type == 2)
		{
			string texture_x;
			string texture_y;

			getline(ss, texture_x, ',');
			getline(ss, texture_y, ',');

			//cout << texture_x << " " << texture_y << endl;

			Vertex& vertex = vertices[index_tex];

			vertex.texCoords.x = stof(texture_x);
			vertex.texCoords.y = stof(texture_y);
			++index_tex;
		}
	}
	return vertices;
}

vector<unsigned int> Renderer::LoadIndices(const string& index_path)
{
	//cout << "Load indices!" << endl;

	vector<unsigned int> indices;

	ifstream index_file(index_path);

	if (!index_file.is_open())
	{
		cout << "Renderer Failed to open the index file " << index_path << endl;
	}

	string line;

	while (getline(index_file, line))
	{
		stringstream ss(line);
		if (line[0] == '#')
		{
			continue;
		}

		string index;

		while (getline(ss, index, ','))
		{
			const char* index_char = index.c_str();
			unsigned int index_uint = strtoul(index_char, nullptr, 0);
			indices.push_back(index_uint);
		}

	}

	return indices;
}

bool Renderer::SetupShadow()
{
	cout << "Setup Quad for drawing framebuffer texture" << endl;

	glGenVertexArrays(1, &m_frame_VAO);
	glGenBuffers(1, &m_frame_VBO);
	glBindVertexArray(m_frame_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_frame_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


	cout << "Setup Depth map" << endl;

	// Framebuffer to store depth values(shadow map)
	glGenFramebuffers(1, &m_shadow_FBO);
	//glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_FBO);

	// Texture for shadow framebuffer
	glGenTextures(1, &m_shadow_map);
	glBindTexture(GL_TEXTURE_2D, m_shadow_map);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	if (glGetError() != GL_NO_ERROR)
	{
		cout << "error " << glGetError() << endl;
		cout << m_shadow_map << endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadow_map, 0);
	
	// Disable writes to color buffer, as shadow map will not output the color
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if ( status != GL_FRAMEBUFFER_COMPLETE)
	{
		cout << "Framebuffer error " << status << endl;
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	return true;
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