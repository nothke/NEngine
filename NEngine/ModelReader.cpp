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
#include <half.hpp>

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
		std::cout << "ERROR: ModelReader:: Can't open file: " << path << std::endl;
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

int from_u16i(char* buffer)
{

}

unsigned short to_ushort(char* buffer)
{
	//unsigned short p;
	//p = (((unsigned short)buffer[1]) << 8) | buffer[0];
	unsigned short p = *reinterpret_cast<unsigned short*>(&buffer[0]);
	return p;
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

float half_to_float(char* buffer)
{
	using half_float::half;
	half h(0);
	unsigned char b[] = { buffer[0], buffer[1] };
	memcpy(&h, &b, sizeof(h));
	return (float)h;
}

float charnorm_to_float(char* buffer)
{
	return (float)buffer[0] / 255.0f;
}

float to_float(char* buffer)
{
	float f;
	unsigned char b[] = { buffer[0], buffer[1], buffer[2], buffer[3] };
	memcpy(&f, &b, sizeof(f));
	return f;
}

bool bit(char c, int i)
{
	return (c >> (7 - i)) & 1;
}

int ModelReader::LoadFromHPM(const std::string& path, Mesh& mesh)
{
	std::cout << "Reading HPM: " << path << std::endl;

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
		std::cout << "ERROR: ModelReader:: Can't open file: " << path << std::endl;
		return -1;
	}

	char buf8[1];
	char buf16[2];
	char buf32[4];

	// flags #1

	file.read(buf8, 1);
	std::cout << "flags#1: " << +static_cast<unsigned char>(buf8[0]) << std::endl;

	bool f00_hasUVs = bit(buf8[0], 0);
	bool f01_hasColors = bit(buf8[0], 1);
	bool f02_32bit_positions = bit(buf8[0], 2);
	bool f03_32bit_indices = bit(buf8[0], 3);
	bool f04_32bit_uvs = bit(buf8[0], 4);
	bool f05_32bit_colors = bit(buf8[0], 5);

	// flags #2

	file.read(buf8, 1);
	std::cout << "flags#2: " << +static_cast<unsigned char>(buf8[0]) << std::endl;

	// unused

	// vertex count - 2 bytes

	file.read(buf16, 2);
	unsigned short vc = to_ushort(buf16);

	// index count - 2 bytes

	file.read(buf16, 2);
	unsigned short ic = to_ushort(buf16);

	std::vector<Vertex> vertices;
	vertices.reserve(vc);

	std::cout << "has uv " << f00_hasUVs << std::endl;
	std::cout << "has colors " << f01_hasColors << std::endl;
	std::cout << "has 32bit positions " << f02_32bit_positions << std::endl;
	std::cout << "has 32bit indices " << f03_32bit_indices << std::endl;
	std::cout << "has 32bit uvs " << f04_32bit_uvs << std::endl;
	std::cout << "has 32bit colors " << f05_32bit_colors << std::endl;

	std::cout << "VC: " << vc << std::endl;
	std::cout << "IC: " << ic << std::endl;

	// half test
	//file.read(buf16, 2);
	//float v = half_to_float(buf16);
	//std::cout << "H2F: " << v << std::endl;

	// half test
	/*
	for (size_t i = 0; i < 65535; i++)
	{
		file.read(buf16, 2);

		unsigned short index = to_ushort(buf16);
		int indexi = index;

		std::cout << "i: " << i << " " << indexi << std::endl;
	}
	*/

	//return -1;

	for (size_t i = 0; i < vc; i++)
	{
		float posx, posy, posz;

		if (f02_32bit_positions)
		{
			file.read(buf32, 4); posx = to_float(buf32);
			file.read(buf32, 4); posy = to_float(buf32);
			file.read(buf32, 4); posz = to_float(buf32);
		}
		else
		{
			file.read(buf16, 2); posx = half_to_float(buf16);
			file.read(buf16, 2); posy = half_to_float(buf16);
			file.read(buf16, 2); posz = half_to_float(buf16);
		}

		float uvs = 0, uvt = 0;

		if (f00_hasUVs)
		{
			if (f04_32bit_uvs)
			{
				file.read(buf32, 4); uvs = to_float(buf32);
				file.read(buf32, 4); uvt = to_float(buf32);
			}
			else
			{
				file.read(buf16, 2); uvs = half_to_float(buf16);
				file.read(buf16, 2); uvt = half_to_float(buf16);
			}
		}

		float colr = 0, colg = 0, colb = 0, cola = 0;

		if (f01_hasColors)
		{
			if (f05_32bit_colors)
			{
				file.read(buf32, 4); colr = to_float(buf32);
				file.read(buf32, 4); colg = to_float(buf32);
				file.read(buf32, 4); colb = to_float(buf32);
				file.read(buf32, 4); cola = to_float(buf32);
			}
			else
			{
				file.read(buf8, 1); colr = charnorm_to_float(buf8);
				file.read(buf8, 1); colg = charnorm_to_float(buf8);
				file.read(buf8, 1); colb = charnorm_to_float(buf8);
				file.read(buf8, 1); cola = charnorm_to_float(buf8);
			}
		}

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
		if (f03_32bit_indices)
		{
			file.read(buf32, 4);

			int index = to_int(buf32);
			indices.push_back(index);

			std::cout << "i: " << i << " " << index << std::endl;
		}
		else
		{
			file.read(buf16, 2);

			unsigned short index = to_ushort(buf16);
			//int indexi = index;
			indices.push_back(index);

			std::cout << "i: " << i << " " << index << std::endl;
		}

		if (!file.good())
		{
			std::cout << "fail at : " << i << std::endl;
			file.close();
			return -1;
		}
	}

	std::cout << "FML" << std::endl;
	std::cout << vertices[0].posx << ", " << vertices[0].colr << std::endl;
	std::cout << indices[0] << std::endl;

	mesh.Init(vertices, indices);

	file.close();

	return 0;
}