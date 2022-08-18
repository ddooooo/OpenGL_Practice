#include "Renderer.h"

#include <fstream>
#include <sstream>

Renderer::Renderer()
{
}

bool Renderer::Initialize(float screen_width, float screen_height)
{
	m_width = screen_width;
	m_height = screen_height;

	SDL_Init(SDL_INIT_VIDEO);

	m_window = SDL_CreateWindow("Model Loading", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		static_cast<int>(screen_width), static_cast<int>(screen_height), SDL_WINDOW_OPENGL);

	// Set core OpenGL profile
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	// Set version of SDL 
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// Set a color buffer with 8 bits for each channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);

	// Set double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Hardware Acceleration
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	if (!m_window)
	{
		printf( "Failed to load window: %s \n", SDL_GetError());
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

	glEnable(GL_DEPTH_TEST);

	printf("%s \n", glGetString(GL_VERSION));

	m_camera = new Camera();

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

	while (SDL_PollEvent(&m_event))
	{
		if (m_event.type == SDL_QUIT)
		{
			m_is_running = 0;
		}
		else if (m_event.type == SDL_MOUSEMOTION)
		{
			m_camera->ProcessMouseMotion(m_event);
		}
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	SetupLight();

	mat4 p = perspective(radians(m_camera->GetFov()), m_width / m_height, 0.1f, 100.0f);
	mat4 v = m_camera->MyLookAt();
	mat4 m = mat4(1.0f);

	//m = translate(m, vec3(0.0f, 0.0f, 0.0f));
	m = scale(m, vec3(0.5f, 0.5f, 0.5f));

	m_model_shader->SetActive();
	m_model_shader->SetMat4("projection", p);
	m_model_shader->SetMat4("view", v);
	m_model_shader->SetMat4("model", m);

	m_model->Draw(*m_model_shader);

	m_light_shader->SetActive();
	m_light_shader->SetMat4("projection", p);
	m_light_shader->SetMat4("view", v);

	glBindVertexArray(m_light_mesh->GetVAO());
	for (int i = 0; i < 4; ++i)
	{
		m = mat4(1.0f);
		m = translate(m, m_light_positions[i]);
		m = scale(m, vec3(0.2f));
		m_light_shader->SetMat4("model", m);
		m_light_mesh->Draw();
	}

	SDL_GL_SwapWindow(m_window);

}

bool Renderer::LoadModel(const string& path, bool flip)
{
	m_model = new Model();

	stbi_set_flip_vertically_on_load(flip);


	return m_model->LoadModel(path);
}

bool Renderer::LoadShader()
{
	cout << "Load Shader!" << endl;

	m_model_shader = new Shader();

	string model_vert_path = "Shaders/1_Model.vert";
	string model_frag_path = "Shaders/1_Model.frag";
	
	if (!m_model_shader->LoadShaderFile(model_vert_path, model_frag_path))
	{
		printf("Failed to load model shader files! \n");
		return false;
	}

	m_light_shader = new Shader();

	string lightVertPath = "Shaders/1_LightColor.vert";
	string lightFragPath = "Shaders/1_LightColor.frag";

	if (!m_light_shader->LoadShaderFile(lightVertPath, lightFragPath))
	{
		printf("Failed to load light shader files! \n");
		return false;
	}

	return true;
}

bool Renderer::LoadVertices()
{
	cout << "Load Vertices!" << endl;

	string vertex_path = "Vertices/Cube_Vertices_List2.txt";
	
	ifstream vertex_file(vertex_path);
	
	if (!vertex_file.is_open())
	{
		cout << "Failed to open the vertex file " << vertex_path << endl;
		return false;
	}

	string line;

	while (getline(vertex_file, line))
	{
		stringstream ss(line);
		if (line[0] == '#')
		{
			continue;
		}
		string vertex_x;
		string vertex_y;
		string vertex_z;
		getline(ss, vertex_x, ',');
		getline(ss, vertex_y, ',');
		getline(ss, vertex_z, ',');

		Vertex vertex;
		//vertex.position = position;
		vertex.position.x = stof(vertex_x);
		vertex.position.y = stof(vertex_y);
		vertex.position.z = stof(vertex_z);



		m_light_vertices.push_back(vertex);
	}

	cout << "Load indices!" << endl;

	string index_path = "Vertices/Cube_Indices_List.txt";

	ifstream index_file(index_path);

	if (!index_file.is_open())
	{
		cout << "Failed to open the index file " << index_path << endl;
	}

	while (getline(index_file, line))
	{
		stringstream ss(line);
		if (line[0] == '#')
		{
			continue;
		}

		string index;
		
		while(getline(ss, index, ','))
		{ 
			const char* index_char = index.c_str();
			unsigned int index_uint = strtoul(index_char, nullptr, 0);
			m_light_indices.push_back(index_uint);
		}

	}

	m_light_mesh = new Mesh(m_light_vertices, m_light_indices);

	m_light_mesh->SetupMesh();
	
	return true;
}

void Renderer::SetupLight()
{
	vec3 cam_pos = m_camera->GetPos();
	vec3 cam_forward = m_camera->GetForward();

	m_model_shader->SetActive();
	m_model_shader->SetVec3("viewPos", cam_pos);

	m_model_shader->SetVec3("dirLight.direction", m_dir);
	m_model_shader->SetVec3("dirLight.ambient", m_amb);
	m_model_shader->SetVec3("dirLight.diffuse", m_diff);
	m_model_shader->SetVec3("dirLight.specular", m_spec);
}

void Renderer::UnLoad()
{
	m_model_shader->UnLoad();
	//m_light_shader->UnLoad();

	//glDeleteBuffers(1, &light_VBO);
	//glDeleteVertexArrays(1, &light_VAO);

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	delete m_camera;
}



