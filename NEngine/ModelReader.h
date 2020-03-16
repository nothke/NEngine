#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"

class ModelReader
{
public:
	int Get(const char* path, Mesh** mesh);
	int Get(const char * path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	ModelReader();
	~ModelReader();
};

