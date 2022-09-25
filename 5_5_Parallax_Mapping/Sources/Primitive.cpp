#include "Primitive.h"
#include <fstream>
#include <sstream>

Primitive::Primitive(Shape shape) : m_shape(shape)
{}

Primitive::~Primitive()
{}

bool Primitive::LoadPrimitive()
{
	cout << "LoadPrimitive " << m_shape << endl;

	string v_path = "";
	string i_path = "";

	switch (m_shape)
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
		return false;
	}

	vector<VertexLayout> vertices = LoadVertices(v_path);
	vector<unsigned int> indices = LoadIndices(i_path);

	if (vertices.empty())
	{
		printf("Renderer Failed to load vertices \n");
		return false;
	}

	if (indices.empty())
	{
		printf("Renderer Failed to load indices \n");
		return false;
	}

	vec3 t;
	if (m_shape == Shape::SQUARE || m_shape == Shape::FLOOR)
	{
		vec3 p1 = vertices[1].position;
		vec3 p2 = vertices[0].position;
		vec3 p3 = vertices[2].position;
	
		vec2 uv1 = vertices[1].texCoords;
		vec2 uv2 = vertices[0].texCoords;
		vec2 uv3 = vertices[2].texCoords;

		vec3 e1 = p2 - p1;
		vec3 e2 = p3 - p1;
		vec2 duv1 = uv2 - uv1;
		vec2 duv2 = uv3 - uv1;

		float f = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
		t.x = f * ((duv2.y * e1.x) - (duv1.y * e2.x));
		t.y = f * ((duv2.y * e1.y) - (duv1.y * e2.y));
		t.z = f * ((duv2.y * e1.z) - (duv1.y * e2.z));
		
		for (auto& it : vertices)
		{
			it.tangent = t;
		}
	}
	
	m_mesh = unique_ptr<Mesh>(new Mesh(vertices, indices));
	m_mesh->SetupMesh();

	return true;
}

void Primitive::SetActive()
{
	m_mesh->SetActive();
}

void Primitive::Draw()
{
	m_mesh->Draw();
}

vector<VertexLayout> Primitive::LoadVertices(const string& vert_path)
{
	vector<VertexLayout> vertices;

	//cout << "Load Vertices!" << endl;

	ifstream vertex_file(vert_path);

	if (!vertex_file.is_open())
	{
		cout << "Failed to open the vertex file " << vert_path << endl;
		return {};
	}

	string line;
	int index_tex = 0;
	int index_nor = 0;
	int type = 0;
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
			type = 0;
			continue;
		}
		else if (line[0] == 'n')
		{
			//cout << line << endl;
			type = 1;
			continue;
		}
		else if (line[0] == 't')
		{
			//cout << "Texture coordinate load" << endl;
			type = 2;
			continue;
		}

		if (type == 0)
		{
			VertexLayout vertex;
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
		else if (type == 1)
		{
			string normal_x;
			string normal_y;
			string normal_z;

			getline(ss, normal_x, ',');
			getline(ss, normal_y, ',');
			getline(ss, normal_z);

			VertexLayout& vertex = vertices[index_nor];

			vertex.normal.x = stof(normal_x);
			vertex.normal.y = stof(normal_y);
			vertex.normal.z = stof(normal_z);

			++index_nor;
		}
		else if (type == 2)
		{
			string texture_x;
			string texture_y;

			getline(ss, texture_x, ',');
			getline(ss, texture_y, ',');

			VertexLayout& vertex = vertices[index_tex];

			vertex.texCoords.x = stof(texture_x);
			vertex.texCoords.y = stof(texture_y);
			++index_tex;
		}
	}

	return vertices;
}

vector<unsigned int> Primitive::LoadIndices(const string& index_path)
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

ostream& operator<<(ostream& os, const Primitive::Shape& shape)
{
	string s = "";
	switch (shape)
	{
	case(Primitive::Shape::SQUARE):
		s = "Square";
		break;

	case(Primitive::Shape::CUBE):
		s = "Cube";
		break;

	case(Primitive::Shape::FLOOR):
		s = "Floor";
		break;
	}

	return os << s;
}
