#include "Renderer.h"

#include <GL/glew.h>
#include "GLAssert.h"

void Renderer::Init()
{
	// Todo: move to renderer
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(60.0f / 255, 195.0f / 255, 1, 1));
}

void Renderer::Clear()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::DrawMesh(Mesh & mesh)
{
	GLCall(glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr));
}

Renderer::Renderer()
{
}


Renderer::~Renderer()
{
}
