#pragma once

#include <string>
#include <vector>

using namespace std;

class Texture
{
public:
	Texture();
	Texture(const string& path);
	Texture(const string& path, const string& type);
	~Texture();

	bool LoadTexture();
	bool LoadCubemapTexture(const vector<string>& faces);

	void Unload();
	void SetActive();
	
	int GetWidth() const { return m_width; };
	int GetHeight() const { return m_height; };
	unsigned int GetTextureID() const { return m_texture_ID; };
	
	string GetPath() const { return m_path; }
	string GetType() const { return m_type; }
private:
	unsigned int m_texture_ID;
	int m_width;
	int m_height;

	string m_path;
	string m_type;
};