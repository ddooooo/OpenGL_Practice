#pragma once

#include <iostream>
#include <string>

#include "Mesh.h"

using namespace std;

class Primitive
{
public:
	Primitive(const string path);
	~Primitive();

	virtual bool Load();

	void SetActive();
	void Draw();

	//friend ostream& operator<<(ostream&, const Shape&);

protected:
	const string m_path;
	unique_ptr<Mesh> m_mesh;
	vector<VertexLayout> m_layouts;
	vector<unsigned int> m_indices;

	bool LoadVertices();
	//vector<unsigned int> LoadIndices(const string& index_path);


};