#pragma once
#include <iostream>
#include <vector>
#include "Vertex.h"

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// TODO: remove
	int vertexCount;
	int indexCount;

	void Bind();

	void Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector, bool optimizeVertexCache = true);
	void Simplify(float threshold, float target_error = 1e-2f);
	void SimplifySloppy(float threshold);

private:
	unsigned int vao;
	unsigned int ibo;

	void CreateAttributes();
};

