#include "Renderer.h"

#include <GL/glew.h>

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
