#pragma once

#include <string>

using namespace std;

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const string& fileName);
	void Unload();

	void SetActive();
	int GetWidth() const { return mWidth; };
	int GetHeight() const { return mHeight; };
	unsigned int GetTextureID() const { return mTexture; };
private:
	unsigned int mTexture;
	int mWidth;
	int mHeight;
};