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
	
	m_window = SDL_CreateWindow("Model Loading", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		static_cast<int>(m_width), static_cast<int>(m_height), SDL_WINDOW_OPENGL);


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

	glEnable(GL_DEPTH_TEST); // Enable depth buffer
	glEnable(GL_STENCIL_TEST); // Enable stencil buffer
	// If both depth and stencil test pass, then use the result of the stencil test
	// Replace with the masked fragment value
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	SortPosition();

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(0);
	// Need to clear color, depth, and stencil buffer for every frame
	// color, depth, and stencil are updated for every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

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

	// #1 Draw a original model
	// glStencilFunc -> stencil test, it always pass the stencil test
	// Set stencil buffer with 1 / 1 & (operation)0xFF -> 1
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	// Enable modifying stencil buffer
	glStencilMask(0xFF); 

	m_link->Draw(*m_model_shader);

	// #2 Draw a larger model with white color
	// The pixel that is NOT equal to 1  will be passed
	// --> Thus only draw a object where the stencil buffer is equal to 0
	// --> And it will only draw border of a model
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); 
	// Disable modifying stencil buffer
	// Stencil buffer will not be updated when drawing larger model
	glStencilMask(0x00);
	// Disable depth test
	glDisable(GL_DEPTH_TEST);
	
	m_outline_shader->SetActive();
	m_outline_shader->SetMat4("projection", p);
	m_outline_shader->SetMat4("view", v);
	//m = scale(m, vec3(1.05f, 1.05f, 1.05f));
	m_outline_shader->SetMat4("model", m);
	m_outline_shader->SetFloat("outline", 0.05f);
	m_link->Draw(*m_outline_shader);

	// #3 Enable modifying stencil buffer
	glStencilMask(0xFF);
	// Set stencil buffer with 0 --> clear stencil buffer
	glStencilFunc(GL_ALWAYS, 0, 0xFF);
	//Enable depth test
	glEnable(GL_DEPTH_TEST);

	// Draw Lights
	m_light_shader->SetActive();
	m_light_shader->SetMat4("projection", p);
	m_light_shader->SetMat4("view", v);

	for (int i = 0; i < 4; ++i)
	{
		m = mat4(1.0f);
		m = translate(m, m_light_positions[i]);
		m = scale(m, vec3(0.2f));
		m_light_shader->SetMat4("model", m);
		m_primitives[0].Draw();
	}

	// Draw grass
	m_grass_shader->SetActive();
	m_grass_shader->SetMat4("projection", p);
	m_grass_shader->SetMat4("view", v);

	//glBindVertexArray(m_primitives[1].GetVAO());
	glBindTexture(GL_TEXTURE_2D, m_grass_texture->GetTextureID());
	for (int i = 0; i < m_vegetations.size(); ++i)
	{
		m = mat4(1.0f);
		m = translate(m, m_vegetations[i]);
		m_grass_shader->SetMat4("model", m);
		m_primitives[1].Draw();
	}

	// Draw windows
	m_window_shader->SetActive();
	m_window_shader->SetMat4("projection", p);
	m_window_shader->SetMat4("view", v);

	int index = 1;

	glBindTexture(GL_TEXTURE_2D, m_window_texture->GetTextureID());
	cout << "size of sorted: " << m_sorted.size() << endl;
	auto it = m_sorted.rbegin();
	for (it; it != m_sorted.rend(); ++it)
	{
		m = mat4(1.0f);
		m = translate(m, it->second);
		//cout << "Position" << index << ": " << it->second.x << " " << it->second.y << " " << it->second.z << endl;
		m_window_shader->SetMat4("model", m);
		m_primitives[1].Draw();
		index++;
	}

	index = 1;

	SDL_GL_SwapWindow(m_window);
}

bool Renderer::LoadModel(const string& path, bool flip)
{
	m_link = new Model();

	stbi_set_flip_vertically_on_load(flip);

	return m_link->LoadModel(path);
}

bool Renderer::LoadShader()
{
	cout << "Load Shader!" << endl;

	m_model_shader = new Shader();

	string vert_path = "Shaders/1_Model.vert";
	string frag_path = "Shaders/1_Model.frag";
	
	if (!m_model_shader->LoadShaderFile(vert_path, frag_path))
	{
		printf("Failed to load model shader files! \n");
		return false;
	}

	m_outline_shader = new Shader();

	vert_path = "Shaders/1_Stencil.vert";
	frag_path = "Shaders/1_Stencil.frag";

	if (!m_outline_shader->LoadShaderFile(vert_path, frag_path))
	{
		printf("Failed to load outline shader files! \n");
		return false;
	}

	m_light_shader = new Shader();

	vert_path = "Shaders/1_LightColor.vert";
	frag_path = "Shaders/1_LightColor.frag";

	if (!m_light_shader->LoadShaderFile(vert_path, frag_path))
	{
		printf("Failed to load light shader files! \n");
		return false;
	}

	m_grass_shader = new Shader();

	vert_path = "Shaders/1_Grass.vert";
	frag_path = "Shaders/1_Grass.frag";

	if (!m_grass_shader->LoadShaderFile(vert_path, frag_path))
	{
		printf("Failed to load grass shader files! \n");
		return false;
	}

	string tex_path = "Textures/grass.png";
	m_grass_texture = new Texture(tex_path);
	m_grass_texture->LoadTexture();

	m_grass_shader->SetActive();
	m_grass_shader->SetInt("texture1", 0);


	m_window_shader = new Shader();

	vert_path = "Shaders/1_Window.vert";
	frag_path = "Shaders/1_Window.frag";

	if (!m_window_shader->LoadShaderFile(vert_path, frag_path))
	{
		printf("Failed to load window shader file! \n");
		return false;
	}

	tex_path = "Textures/Window.png";
	m_window_texture = new Texture(tex_path);
	m_window_texture->LoadTexture();

	m_window_shader->SetActive();
	m_window_shader->SetInt("tetxture1", 0);


	return true;
}

bool Renderer::LoadPrimitive(const Shape& shape)
{
	cout << "Load Primitive!" << endl;

	string vertex_path = "";
	string index_path = "";
	
	switch (shape)
	{
	case(Shape::BOX):
		vertex_path = "Vertices/Box/Box_Vertices_List.txt";
		index_path = "Vertices/Box/Box_Indices_List.txt";
		break;

	case(Shape::CUBE):
		vertex_path = "Vertices/Cube/Cube_Vertices_List.txt";
		index_path = "Vertices/Cube/Cube_Indices_List.txt";
		break;
		
	default:
		cout << "Invalide Shape!" << endl;
		return false;
	}

	Mesh primitive(LoadVertices(vertex_path), LoadIndices(index_path));


	primitive.SetupMesh();

	m_primitives.push_back(primitive);

	for (int i = 0; i < m_primitives.size(); ++i)
	{
		cout << "Primitive: " << m_primitives[i].m_vertices.size() << endl;
		for (int j = 0; j < m_primitives[i].m_vertices.size(); ++j)
		{
			cout << "TexCoord: " << m_primitives[i].m_vertices[j].texCoords.x << " "
				<< m_primitives[i].m_vertices[j].texCoords.y << endl;
		}
	}

	return true;
}

vector<Vertex> Renderer::LoadVertices(const string& vert_path)
{
	vector<Vertex> vertices;

	cout << "Load Vertices!" << endl;
	
	ifstream vertex_file(vert_path);
	
	if (!vertex_file.is_open())
	{
		cout << "Failed to open the vertex file " << vert_path << endl;
		return {};
	}

	string line;
	int index = 0;
	while (getline(vertex_file, line))
	{
		stringstream ss(line);
		if (line[0] == '#')
		{
			continue;
		}

		
		if (line[0] == 'v')
		{
			cout << "Vertex load" << endl;
			m_type = 0;
			continue;
		}
		else if (line[0] == 't')
		{
			cout << "texture coordinate load" << endl;
			m_type = 1;
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

			cout << vertex_x << " " << vertex_y << " " << vertex_z << endl;

			//vertex.position = position;
			vertex.position.x = stof(vertex_x);
			vertex.position.y = stof(vertex_y);
			vertex.position.z = stof(vertex_z);
			vertices.push_back(vertex);
		}
		else if (m_type == 1)
		{
			string texture_x;
			string texture_y;

			getline(ss, texture_x, ',');
			getline(ss, texture_y, ',');

			cout << texture_x << " " << texture_y << endl;

			Vertex& vertex = vertices[index];

			vertex.texCoords.x = stof(texture_x);
			vertex.texCoords.y = stof(texture_y);
			index++;
		}
	}

	//m_light_mesh = new Mesh(m_light_vertices, m_light_indices);

	//m_light_mesh->SetupMesh();
	
	return vertices;
}

vector<unsigned int> Renderer::LoadIndices(const string& index_path)
{
	cout << "Load indices!" << endl;

	vector<unsigned int> indices;

	ifstream index_file(index_path);

	if (!index_file.is_open())
	{
		cout << "Failed to open the index file " << index_path << endl;
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
	//m_model_shader->UnLoad();
	//m_light_shader->UnLoad();
	//m_outline_shader->UnLoad();

	// Terminate and clear all allocated SDL resources
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
	SDL_Quit();

	delete m_link;
	//delete m_model_outline;
	
	delete m_camera;
	
	delete m_model_shader;
	delete m_light_shader;
	delete m_outline_shader;
}

void Renderer::SortPosition()
{
	if (!m_sorted.empty())
		m_sorted.clear();

	for (int i = 0; i < m_windows.size(); ++i)
	{
		float distance = length2(m_camera->GetPos() - m_windows[i]);
		m_sorted[distance] = m_windows[i];
	}
}

