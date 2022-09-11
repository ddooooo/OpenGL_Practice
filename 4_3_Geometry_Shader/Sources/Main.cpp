#include "Renderer.h"

using namespace std;
using namespace glm;

int main(int argc, char** argv)
{
	Renderer* renderer = new Renderer();

	if (!renderer->Initialize(800.0f, 600.0f))
	{
		cout << "Failed to initialize!" << endl;
		return 0;
	}

	while (renderer->IsRunning())
	{
		renderer->Draw();
	}

	// Terminate and clear all allocated SDL resources
	renderer->UnLoad();

	return 0;
}