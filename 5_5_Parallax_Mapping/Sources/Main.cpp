#include "Renderer.h"

using namespace std;

int main(int argc, char** argv)
{
	Renderer* renderer = new Renderer();

	if (!renderer->Initialize(800.0f, 600.0f))
	{
		cerr << "Failed to initialize!" << endl;
		return 0;
	}

	renderer->Run();
	
	// Terminate and clear all allocated SDL resources
	renderer->UnLoad();

	return 0;
}