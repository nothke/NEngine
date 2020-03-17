#include "Renderer.h"

#include <GL/glew.h>

void Renderer::Init()
{
	// Todo: move to renderer
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(60.0f / 255, 195.0f / 255, 1, 1);
}

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawMesh(Mesh & mesh)
{
	glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr);
}

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}
