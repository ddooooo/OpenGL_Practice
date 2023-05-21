#include "Mesh.h"
#include <fstream>
#include <sstream>

using namespace std;

Mesh::Mesh() : m_buffers(make_unique<VertexBuffer>()), m_layouts({}), m_indices({}) {}

bool Mesh::importMeshFromFile(const string& file_path)
{
	string::size_type pos = file_path.find_last_of('.');
	if (pos != file_path.length())
	{
		string file_type = file_path.substr(pos+1, file_path.length());
		cout << file_type << endl;
		if (file_type == "txt")
		{
			return loadText(file_path);
		}
	}

	return false;
}

bool Mesh::loadText(const string& file_path)
{
	ifstream vertex_file(file_path);

	cout << "Load mesh from text file: " << file_path << endl;

	if (!vertex_file.is_open())
	{
		cout << "Failed to open the vertex file " << file_path << endl;
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

	for (int i = 0; i < positions.size(); ++i)
	{
		info::VertexLayout layout;
		layout.position = positions[i];
		//layout.normal = normals[i];
		//layout.texCoord = texCoords[i];

		m_layouts.push_back(layout);
	}

	loadMeshToBuffers();

	return true;
}

void Mesh::loadMeshToBuffers()
{
	m_buffers->createBuffers(m_layouts, m_indices);
}

void Mesh::draw()
{
	bind();
	glDrawElements(GL_TRIANGLES, (int)m_indices.size(), GL_UNSIGNED_INT, nullptr);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	unbind();
}

void Mesh::bind() const
{
	m_buffers->bind();
}

void Mesh::unbind() const
{
	m_buffers->unbind();
}