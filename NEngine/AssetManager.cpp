#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ModelReader.h"
#include <filesystem>

AssetManager::AssetManager(const int meshesCapacity, const int texturesCapacity, const int shadersCapacity)
{
	meshes.reserve(meshesCapacity);
	meshNames.reserve(meshesCapacity);

	textures.reserve(texturesCapacity);

	shaders.reserve(shadersCapacity);
	shaderPaths.reserve(shadersCapacity);
}

void AssetManager::AddMesh(const Mesh& mesh, const char* name)
{
	meshes.push_back(mesh);
	meshNames.push_back(name);
}

int AssetManager::GetMeshIndex(const std::string & name)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		if (meshNames[i] == name)
			return i;
	}

	return -1;
}

Mesh& AssetManager::GetMesh(int i)
{
	return meshes[i];
}

Mesh& AssetManager::GetMesh(const std::string& name)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		if (meshNames[i] == name)
		{
			return meshes[i];
		}
	}
}

Mesh& AssetManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const char* name)
{
	meshes.emplace_back(vertices, indices, true);
	meshNames.push_back(name);
	return meshes[meshes.size() - 1];
}

Mesh& AssetManager::CreateMesh(const char * path)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ModelReader::LoadFromPly(path, vertices, indices);
	meshes.emplace_back(vertices, indices, true);

	std::filesystem::path filepath(path);
	auto name = filepath.stem().string();
	meshNames.push_back(name);
	std::cout << "Mesh: " << name << std::endl;

	return meshes[meshes.size() - 1];
}

std::optional<std::reference_wrapper<Texture>> AssetManager::GetTexture(const char * name)
{
	for (size_t i = 0; i < textures.size(); i++)
	{
		if (textureNames[i] == name)
			return textures[i];
	}
	return {};
}

Texture & AssetManager::CreateTexture(const char * path, Texture::Filtering filtering, Texture::EdgeMode edgeMode)
{
	//Texture tex(path, filtering, edgeMode);
	textures.emplace_back(path, filtering, edgeMode);// tex);

	std::filesystem::path filepath(path);
	auto name = filepath.stem().string();
	textureNames.push_back(name);

	std::cout << "Created texture: " << name << std::endl;

	return textures[textures.size() - 1];
}

Shader & AssetManager::CreateShader(const char * path)
{
	ShaderSource source = ShaderReader::Parse(path);
	Shader shader(source);
	shaders.push_back(source);
	shaderPaths.push_back(path);

	return shaders[shaders.size() - 1];
}

void AssetManager::RebuildAll()
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

void AssetManager::ReloadShaders()
{
	for (size_t i = 0; i < shaders.size(); i++)
	{
		auto source = ShaderReader::Parse(shaderPaths[i]);
		shaders[i] = Shader(source);
	}
}

void AssetManager::Dispose()
{
	for (Shader& s : shaders)
		s.Delete();

	// Meshes?

	for (Texture& t : textures)
		t.Release();
}