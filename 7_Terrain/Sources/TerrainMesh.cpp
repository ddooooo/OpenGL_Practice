#include "TerrainMesh.h"

TerrainMesh::TerrainMesh(string name, int width, int height, vector<VertexLayout> vertices, vector<unsigned int> indices)
	: Mesh(name, vertices, indices), m_width(width), m_height(height)
{
	cout << "Load Terrain" << endl;
	cout << "Loaded height map with " << width << " width and " << height << " height" << endl;

	n_strips = m_height - 1;
	n_triangles_per_strips = m_width * 2 - 2;

	cout << "Loaded " << vertices.size() << " vertices" << endl;
	cout << "Loaded " << indices.size() << " indices" << endl;

	cout << "Created " << n_strips << " strips with " << n_triangles_per_strips << " triangles for each strip" << endl;
	cout << "Created " << n_strips * n_triangles_per_strips << " triangles total" << endl;

	cout << "Vertices" << endl;
	cout << "Check y: " << vertices.at(0).position.y << endl;

	SetupMesh();
}

void TerrainMesh::Draw()
{
	//cout << "Draw in Terrain Mesh" << endl;

	glBindVertexArray(m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	int n_indices_per_strip = m_width * 2;
	for (unsigned int i = 0; i < n_strips; ++i)
	{
		glDrawElements(GL_TRIANGLE_STRIP, n_indices_per_strip, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)*(n_indices_per_strip)*i));
	}
}

