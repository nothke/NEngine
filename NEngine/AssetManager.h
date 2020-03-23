#pragma once
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "ModelReader.h"

class AssetManager
{
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Shader> shaders;

public:
	Mesh& CreateMesh(const char* path)
	{
		Mesh mesh;
		ModelReader::LoadFromPly(path, mesh);
		// There is a copy here
		meshes.push_back(mesh);

		return meshes[meshes.size() - 1];

		// mesh gets destroyed
	}

	Texture& CreateTexture(const char* path)
	{
		Texture tex(path);
		textures.push_back(tex);

		return textures[textures.size() - 1];
	}

	Shader& CreateShader(const char* path)
	{
		ShaderSource source = ShaderReader::Parse(path);
		Shader shader(source);
		shaders.push_back(source);

		return shaders[shaders.size() - 1];
	}

	void RebuildAll()
	{
		for (Shader& s : shaders)
		{
			s.Recompile();
		}

		for (Texture& t : textures)
		{
			t.Rebuild();
		}

		for (Mesh& mesh : meshes)
		{
			mesh.Rebuild();
		}
	}

	void Dispose()
	{
		for (Shader& s : shaders)
			s.Delete();

		for (Texture& t : textures)
			t.Release();
	}

	AssetManager(const int meshesCapacity, const  int texturesCapacity, const  int shadersCapacity)
	{
		meshes.reserve(meshesCapacity);
		textures.reserve(texturesCapacity);
		shaders.reserve(shadersCapacity);
	}
};