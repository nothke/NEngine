#pragma once
#include <vector>
#include "Vertex.h"

class ModelReader
{
public:
	int Get(const char * path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	ModelReader();
	~ModelReader();
};

