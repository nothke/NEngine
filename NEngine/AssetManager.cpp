#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ModelReader.h"

AssetManager::AssetManager(const int meshesCapacity, const int texturesCapacity, const int shadersCapacity)
{
	meshes.reserve(meshesCapacity);
	textures.reserve(texturesCapacity);
	shaders.reserve(shadersCapacity);
	shaderPaths.reserve(shadersCapacity);
}

void AssetManager::AddMesh(const Mesh& mesh)
{
	meshes.push_back(mesh);
}

Mesh& AssetManager::CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	meshes.emplace_back(vertices, indices, true);
	return meshes[meshes.size() - 1];
}

Mesh & AssetManager::CreateMesh(const char * path)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	ModelReader::LoadFromPly(path, vertices, indices);
	meshes.emplace_back(vertices, indices, true);

	return meshes[meshes.size() - 1];
}

Texture & AssetManager::CreateTexture(const char * path, Texture::Filtering filtering, Texture::EdgeMode edgeMode)
{
	//Texture tex(path, filtering, edgeMode);
	textures.emplace_back(path, filtering, edgeMode);// tex);

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