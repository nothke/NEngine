#include "pch.h"
#include "ModelReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Vertex.h"

//#define DEBUG

#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x)
#endif

#define ERROR(x) std::cout << "ModelReader ERROR: " << x << std::endl

int ModelReader::Get(const char * path, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	std::string line;
	std::ifstream file(path);

	bool headerStart = false;
	bool trisStart = false;

	//std::vector<Vertex> vertices;
	//std::vector<unsigned int> indices;

	if (file.is_open())
	{
		int vertexCount = 0;
		int indexCount = 0;

		while (std::getline(file, line))
		{
			LOG(line);

			int ct = 0;

			// Vertices
			if (headerStart && !trisStart)
			{
				Vertex vert;

				std::string str;
				for (char c : line)
				{
					if (c == ' ')
					{
						float f = std::stof(str);

						switch (ct) {
						case 0: LOG("----- V1: " << f); vert.posx = f; break;
						case 1: LOG("----- V2: " << f); vert.posy = f; break;
						case 2: LOG("----- V3: " << f); vert.posz = f; break;
						case 3: LOG("----- R: " << f); vert.colr = f / 255.0f; break;
						case 4: LOG("----- G: " << f); vert.colg = f / 255.0f; break;
						case 5: LOG("----- B: " << f); vert.colb = f / 255.0f; break;
						}

						if (ct == 5)
							break;

						str.clear();
						ct++;
					}
					str += c;
				}

				vertices.push_back(vert);

				vertexCount--;
				if (vertexCount == 0)
				{
					LOG("----- END OF VERTEX -----");
					trisStart = true;
					continue;
				}
			}

			// Indices
			if (trisStart)
			{
				unsigned int tri[3];

				std::string str;
				for (char c : line)
				{
					if (c == ' ' || c == '\n')
					{
						int i = std::stoi(str);

						switch (ct) {
						case 0:
							if (i != 3)
							{
								ERROR("Engine only accepts meshes with 3 indices");
								return -1;
							}
							break;

						case 1: LOG("----- i0: " << i); tri[0] = i; break;
						case 2: LOG("----- i1: " << i); tri[1] = i; break;
						case 3: LOG("----- i2: " << i); tri[2] = i; break;
						}

						//if (ct == 3)
						//	break;

						str.clear();
						ct++;
					}
					str += c;
				}

				// duplicate for the last piece, because loop ends, not very nice
				float f = std::stoi(str);
				LOG("----- i2: " << f); tri[2] = f;

				indices.push_back(tri[0]);
				indices.push_back(tri[1]);
				indices.push_back(tri[2]);
			}

			if (!headerStart)
			{
				if (line.rfind("element vertex ") == 0)
				{
					std::string num = line.substr(14, std::string::npos);
					vertexCount = std::stoi(num);
					LOG("----- FOUND VCOUNT " << vertexCount);
				}

				if (line.rfind("element face ") == 0)
				{
					std::string num = line.substr(12, std::string::npos);
					indexCount = std::stoi(num);
					LOG("----- FOUND ICOUNT " << indexCount);
				}
			}

			if (!headerStart && line.rfind("end_header") == 0)
			{
				headerStart = true;
				LOG("----- HEADER END, STARTED READING VERTICES");
			}
		}
		file.close();
	}
	else std::cout << "Can't open file";

	LOG(" ");
	LOG("Vertex check: ");

	for (Vertex vert : vertices)
	{
		LOG("V " <<
			vert.posx << ", " <<
			vert.posy << ", " <<
			vert.posz << ", " <<
			vert.colr << ", " <<
			vert.colg << ", " <<
			vert.colb);
	}

	LOG(" ");
	LOG("Index check: ");

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		LOG("I " << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2]);
	}

	return 0;
}

ModelReader::ModelReader()
{
}


ModelReader::~ModelReader()
{
}
