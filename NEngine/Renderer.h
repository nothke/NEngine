#pragma once
#include "Mesh.h"

class Renderer
{
public:
	void Clear();
	void DrawMesh(Mesh& mesh);

	Renderer();
	~Renderer();
};

