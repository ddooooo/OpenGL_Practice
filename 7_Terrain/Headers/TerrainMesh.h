#pragma once

#ifndef TERRAINMESH_H
#define TERRAINMESH_H

#include "Mesh.h"

class TerrainMesh : public Mesh
{
private:
	int m_width;
	int m_height;

	int n_strips;
	int n_triangles_per_strips;

public:
	TerrainMesh(string name, int width, int height, vector<VertexLayout> vertices, vector<unsigned int> indices);

	virtual void Draw() override;
};

#endif