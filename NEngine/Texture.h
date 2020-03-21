#pragma once

#include "Renderer.h"
#include "GLAssert.h"
#include "stb_image/stb_image.h"

class Texture
{
public:
	Texture(const std::string& path)
		: filePath(path)
	{
		stbi_set_flip_vertically_on_load(1);

		buffer = stbi_load(&path[0], &width, &height, &bbp, 4);

		GLCall(glGenTextures(1, &id));
		GLCall(glBindTexture(GL_TEXTURE_2D, id));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
		// unbind
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));

		if (buffer)
			stbi_image_free(buffer);

		std::cout << "Loaded texture: " << path << std::endl;
	}

	~Texture()
	{
		glDeleteTextures(1, &id);
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

private:
	unsigned int id;
	std::string filePath;
	unsigned char* buffer;
	int width;
	int height;
	int bbp;
};

