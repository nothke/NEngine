#pragma once
#include "Mesh.h"

class Renderer
{
public:
	void Init();
	void Clear() const;
	void DrawMesh(const Mesh& mesh); //const;

	Renderer();
	~Renderer();
};

