#pragma once
#include <vector>
#include "Vertex.h"

class ModelReader
{
public:
	std::vector<Vertex> Get(const char * path, std::vector<unsigned int>& indices);

	ModelReader();
	~ModelReader();
};

