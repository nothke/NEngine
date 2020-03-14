#include "pch.h"
#include "ModelReader.h"
#include <iostream>
#include <fstream>
#include <string>

int ModelReader::Get(const char * name)
{
	std::string line;
	std::ifstream file(name);

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::cout << line << '\n';
			if (line.rfind("Se us") == 0)
				std::cout << "Correct!" << '\n';
		}
		file.close();
	}
	else std::cout << "Can't open file";

	return 0;
}

ModelReader::ModelReader()
{
}


ModelReader::~ModelReader()
{
}
