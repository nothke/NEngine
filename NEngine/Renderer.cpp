#include "Renderer.h"
#include <GL/glew.h>
#include "GLAssert.h"

void Renderer::Init()
{
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(60.0f / 255, 195.0f / 255, 1, 1));
}

void Renderer::Clear(glm::vec4 color) const
{
	GLCall(glClearColor(color.r, color.g, color.b, color.a));
	Clear();
}

void Renderer::Clear() const
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::DrawMesh(const Mesh& mesh) //const
{
	GLCall(glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, nullptr));
}