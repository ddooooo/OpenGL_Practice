#pragma once

#include <iostream>

#include <SDL.h>
#include <GL/glew.h>

using namespace std;

class SDL_GL_Window
{
private:
	SDL_Window* m_window;
	SDL_GLContext m_context;

	int m_width;
	int m_height;
	string m_title;

public:
	SDL_GL_Window();
	void init(const int screen_width, const int screen_height, string title);
	void clearWindow();
	void swapWindow();
	void unload();
};