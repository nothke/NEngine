#pragma once
#include <vector>
#include "Vertex.h"

class ModelReader
{
public:
	std::vector<Vertex> Get(const char * name);

	ModelReader();
	~ModelReader();
};

