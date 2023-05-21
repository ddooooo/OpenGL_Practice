#include "Primitive.h"
#include <fstream>
#include <sstream>

Primitive::Primitive(const string path) : m_path(path), m_mesh(nullptr) {}

Primitive::~Primitive() {}

bool Primitive::Load()
{
	cout << "Load Primitive from" << m_path << endl;

	LoadVertices();

	m_mesh = unique_ptr<Mesh>(new Mesh("Primitive", m_layouts, m_indices));
	m_mesh->SetupMesh();

	return true;
}

bool Primitive::LoadVertices()
{
	ifstream vertex_file(m_path);

	if (!vertex_file.is_open())
	{
		cout << "Failed to open the vertex file " << m_path << endl;
		return false;
	}

	string line;
	string type, x, y, z;

	vector<glm::vec3> positions;
	vector<glm::vec3> normals;
	vector<glm::vec2> texCoords;

	while (getline(vertex_file, line))
	{
		if (line.empty()) continue;

		stringstream ss(line);

		getline(ss, type, ' ');

		if (type == "#") continue;

		if (type == "v") // vertex coordinates
		{
			getline(ss, x, ' ');
			getline(ss, y, ' ');
			getline(ss, z, ' ');

			glm::vec3 position = { stof(x), stof(y), stof(z) };
			positions.push_back(position);
		}
		else if (type == "vn")
		{
			getline(ss, x, ' ');
			getline(ss, y, ' ');
			getline(ss, z, ' ');

			glm::vec3 normal = { stof(x), stof(y), stof(z) };
			normals.push_back(normal);
		}
		else if (type == "vt")
		{
			getline(ss, x, ' ');
			getline(ss, y, ' ');

			glm::vec2 texCoord = { stof(x), stof(y) };
			texCoords.push_back(texCoord);
		}
		else if (type == "f")
		{
			string index;
			while (getline(ss, index, ','))
			{
				unsigned int index_uint = stoul(index, nullptr, 0);
				m_indices.push_back(index_uint);
			}
		}
	}

	VertexLayout layout;
	for (int i = 0; i < positions.size(); ++i)
	{
		layout.position = positions[i];
		layout.normal = normals[i];
		layout.texCoords = texCoords[i];

		m_layouts.push_back(layout);
	}

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