#pragma once
#include "Mesh.h"
#include <glm/vec4.hpp>

// Doesn't need to be a class since it has no state... Yet?
class Renderer
{
public:
	void Init();
	void Clear(glm::vec4 color) const;
	void Clear() const;
	void DrawMesh(const Mesh& mesh); //const ?
};