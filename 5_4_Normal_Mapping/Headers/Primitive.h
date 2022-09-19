#pragma once

#include <iostream>
#include <string>

#include "Mesh.h"

using namespace std;

class Primitive
{
public:
	enum class Shape
	{
		SQUARE = 0,
		CUBE,
		FLOOR,
	};

	Primitive(Shape shape);
	~Primitive();

	bool LoadPrimitive();

	void SetActive();
	void Draw();

	friend ostream& operator<<(ostream&, const Shape&);
private:
	unique_ptr<Mesh> m_mesh;
	
	vector<VertexLayout> LoadVertices(const string& vert_path);
	vector<unsigned int> LoadIndices(const string& index_path);
	
	Shape m_shape;
	const string m_vert_path;
	const string m_index_path;
};

ostream& operator<<(ostream& os, const Primitive::Shape& shape);