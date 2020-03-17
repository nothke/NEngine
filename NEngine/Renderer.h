#pragma once
#include "Mesh.h"

class Renderer
{
public:
	void Init();
	void Clear();
	void DrawMesh(Mesh& mesh);

	Renderer();
	~Renderer();
};

