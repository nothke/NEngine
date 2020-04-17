#include "pch.h"
#include "ModelReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Vertex.h"
#include "Mesh.h"
#include <sys/stat.h>

//#define DEBUG

#ifdef DEBUG
#define LOG(x) std::cout << x << std::endl
#else
#define LOG(x)
#endif

#define ERROR(x) std::cout << "ModelReader ERROR: " << x << std::endl

int ModelReader::LoadFromPly(const char* path, Mesh& mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	if (LoadFromPly(path, vertices, indices))
		return -1;

	mesh.Init(vertices, indices);

	return 0;
}

int ModelReader::LoadFromPly(const char* path,
	std::vector<Vertex>& vertices,
	std::vector<unsigned int>& indices)
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

						case 3: LOG("----- S: " << f); vert.uvs = f; break;
						case 4: LOG("----- T: " << f); vert.uvt = f; break;

						case 5: LOG("----- R: " << f); vert.colr = f / 255.0f; break;
						case 6: LOG("----- G: " << f); vert.colg = f / 255.0f; break;
						case 7: LOG("----- B: " << f); vert.colb = f / 255.0f; break;
						}

						if (ct == 7)
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
				int i = std::stoi(str);
				LOG("----- i2: " << i); tri[2] = i;

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
	else
	{
		std::cout << "Can't open file: " << path << std::endl;
		return -1;
	}

#ifdef DEBUG
	LOG(" ");
	LOG("Vertex check: ");

	for (Vertex vert : vertices)
	{
		LOG("V " <<
			vert.posx << ", " <<
			vert.posy << ", " <<
			vert.posz << ", " <<

			vert.uvs << ", " <<
			vert.uvt << ", " <<

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
#endif

	return 0;
}

int to_int(char* buffer)
{
	int a = int(
		(unsigned char)(buffer[3]) << 24 |
		(unsigned char)(buffer[2]) << 16 |
		(unsigned char)(buffer[1]) << 8 |
		(unsigned char)(buffer[0]));

	return a;
}

float to_float(char* buffer)
{
	float f;
	unsigned char b[] = { buffer[0], buffer[1], buffer[2], buffer[3] };
	memcpy(&f, &b, sizeof(f));
	return f;
}

int ModelReader::LoadFromHPM(const std::string& path, Mesh& mesh)
{
	std::cout << "Reading HPM" << std::endl;

	std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
	long fileSize = in.tellg();

	if (fileSize == 0)
	{
		std::cout << "HPM:: file is empty" << std::endl;
		return -1;
	}

	std::ifstream file(path, std::ifstream::binary);

	if (!file.is_open())
	{
		std::cout << "Can't open file: " << path << std::endl;
		return -1;
	}

	char buf32[4];

	file.read(buf32, 4);
	int vc = to_int(buf32);

	file.read(buf32, 4);
	int ic = to_int(buf32);

	std::vector<Vertex> vertices;
	vertices.reserve(vc);

	std::cout << "VC: " << vc << std::endl;
	std::cout << "IC: " << ic << std::endl;

	for (size_t i = 0; i < vc; i++)
	{
		file.read(buf32, 4); float posx = to_float(buf32);
		file.read(buf32, 4); float posy = to_float(buf32);
		file.read(buf32, 4); float posz = to_float(buf32);

		file.read(buf32, 4); float uvs = to_float(buf32);
		file.read(buf32, 4); float uvt = to_float(buf32);

		file.read(buf32, 4); float colr = to_float(buf32);
		file.read(buf32, 4); float colg = to_float(buf32);
		file.read(buf32, 4); float colb = to_float(buf32);
		file.read(buf32, 4); float cola = to_float(buf32);

		Vertex v = { posx, posy, posz, uvs, uvt, colr, colg, colb }; // TODO: a is missing
		vertices.emplace_back(v);

		std::cout << "v: " << posx << " " << posy << " " << posz << " " <<
			uvs << " " << uvt << " " <<
			colr << " " << colg << " " << colb << " " << cola << std::endl;
	}

	std::vector<unsigned int> indices;
	indices.reserve(ic);

	for (size_t i = 0; i < ic; i++)
	{
		file.read(buf32, 4);

		if (!file.good())
		{
			std::cout << "fail at : " << i << std::endl;
			file.close();
			return -1;
		}

		int index = to_int(buf32);
		indices.push_back(index);

		std::cout << "i: " << i << " " << index << std::endl;
	}

	mesh.Init(vertices, indices);

	file.close();

	return 0;
}