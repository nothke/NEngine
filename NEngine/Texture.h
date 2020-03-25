#pragma once

#include "Renderer.h"
#include "GLAssert.h"
#include "stb_image/stb_image.h"
#include <string>

class Texture
{
public:
	enum Filtering { Nearest, Linear };
	enum EdgeMode { Clamp, Wrap };

private:
	unsigned int id;
	unsigned char* buffer;
	int width;
	int height;
	int bbp;

	Filtering filtering;
	EdgeMode edgeMode;

public:

	void Rebuild()
	{
		Release();

		Build();
	}

	void Build()
	{
		stbi_set_flip_vertically_on_load(1);

		buffer = stbi_load(&filePath[0], &width, &height, &bbp, 4);

		if (!buffer)
		{
			std::cout << "ERROR: Failed loading texture: " << filePath << std::endl;
			return;
		}

		GLCall(glGenTextures(1, &id));

		// bind
		GLCall(glBindTexture(GL_TEXTURE_2D, id));

		auto _filtering = filtering == Nearest ? GL_NEAREST : GL_LINEAR;
		auto _wrap = edgeMode == Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT;

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _filtering)); // GL_LINEAR
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _filtering)); // GL_LINEAR
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrap));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrap));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
		// unbind
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));

		if (buffer)
			stbi_image_free(buffer);


	}

	Texture(const std::string& path, Filtering filtering = Nearest, EdgeMode edgeMode = Clamp)
		: id(0), filePath(path), buffer(nullptr),
		width(0), height(0), bbp(0),
		filtering(filtering), edgeMode(edgeMode)
	{
		Build();

		if (width != 0 && height != 0)
			std::cout << "CREATED texture: " << path << ", size: " << width << "x" << height << std::endl;
	}

	void Release()
	{
		glDeleteTextures(1, &id);
		std::cout << "RELEASED texture: " << filePath << std::endl;
	}

	~Texture()
	{
		std::cout << "DISPOSED texture: " << filePath << std::endl;
	};

	void Bind(unsigned int slot = 0) const
	{
		GLCall(glActiveTexture(GL_TEXTURE0 + slot));
		GLCall(glBindTexture(GL_TEXTURE_2D, id));
	}

	void Unbind() const
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	};

	std::string filePath;


};

