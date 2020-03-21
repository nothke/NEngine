#pragma once
#include <iostream>
#include <vector>
#include "Vertex.h"

struct Mesh
{
	Vertex* vertices;
	unsigned int* indices;

	int vertexCount;
	int indexCount;

	void Bind();

	void Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector, bool optimizeVertexCache = true);
	void Simplify(float threshold, float target_error = 1e-2f);
	//void SimplifySloppy();

	Mesh();
	~Mesh();
};

