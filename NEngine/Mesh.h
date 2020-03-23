#pragma once
#include <iostream>
#include <vector>
#include "Vertex.h"
#include <glm/vec3.hpp>

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// TODO: remove
	int vertexCount;
	int indexCount;

	glm::vec3 boundsMin;
	glm::vec3 boundsMax;

private:
	unsigned int vao;
	unsigned int ibo;

	// Methods:
public:
	void Bind();
	void Rebuild();

	void Init(std::vector<Vertex>& vertVector, std::vector<unsigned int>& indicesVector, bool optimizeVertexCache = true);
	void Simplify(float threshold, float target_error = 1e-2f);
	void SimplifySloppy(float threshold);

	Mesh()
	{
		std::cout << "Crated Mesh " << std::endl;
	}

	~Mesh()
	{
		std::cout << "Destroyed Mesh" << std::endl;
	}

	void Debug()
	{
		std::cout << "Mesh: vertices: " << vertexCount << ", indices: " << indexCount << std::endl;
	}

private:
	void CreateAttributes();
	void FindBounds();
};

