#pragma once

#include <string>

using namespace std;

class Texture
{
public:
	Texture();
	Texture(const string& path, const string& type);
	~Texture();

	bool LoadTexture();
	void Unload();
	void SetActive();
	
	int GetWidth() const { return m_width; };
	int GetHeight() const { return m_height; };
	unsigned int GetTextureID() const { return m_texture_ID; };
	
	string GetType() const { return m_type; }

	string GetPath() const { return m_path; }

private:
	unsigned int m_texture_ID;
	int m_width;
	int m_height;

	string m_type;
	string m_path;
};