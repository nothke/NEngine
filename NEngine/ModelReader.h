#pragma once
#include <vector>
#include "Vertex.h"
#include "Mesh.h"

namespace ModelReader
{
	int LoadFromPly(const char * path, Mesh & mesh);
	int LoadFromPly(const char * path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);

	int LoadFromHPM(const std::string& path, Mesh& mesh);
}