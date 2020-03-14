#include "pch.h"
#include "ModelReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Vertex.h"

#define LOG(x) std::cout << x << std::endl

int ModelReader::Get(const char * name)
{
	std::string line;
	std::ifstream file(name);

	bool headerStart = false;
	bool trisStart = false;

	std::vector<Vertex> vertices;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::cout << line << '\n';

			int ct = 0;

			if (headerStart)
			{
				Vertex vert;

				std::string str;
				for (char c : line)
				{
					if (c == ' ')
					{
						float f = std::stof(str);

						switch (ct) {
						case 0: LOG("V1: " << f); vert.posx = f; break;
						case 1: LOG("V2: " << f); vert.posy = f; break;
						case 2: LOG("V3: " << f); vert.posz = f; break;
						case 3: LOG("R: " << f); vert.colr = f / 255.0f; break;
						case 4: LOG("G: " << f); vert.colg = f / 255.0f; break;
						case 5: LOG("B: " << f); vert.colb = f / 255.0f; break;
						}

						if (ct == 5)
							break;

						str.clear();
						ct++;
					}
					str += c;
				}

				if (ct < 5)
				{
					trisStart = true;
					LOG("Bail");
					break;
				}
				else
				{
					vertices.push_back(vert);
				}
			}

			if (!headerStart && line.rfind("end_header") == 0)
			{
				headerStart = true;
				std::cout << "Start read from here" << '\n';
			}
		}
		file.close();
	}
	else std::cout << "Can't open file";

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

	return 0;
}

ModelReader::ModelReader()
{
}


ModelReader::~ModelReader()
{
}
