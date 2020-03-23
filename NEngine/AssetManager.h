#pragma once
#include "Texture.h"
struct Mesh;
class Shader;

class AssetManager
{
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

public:
	std::vector<Shader> shaders;

	AssetManager(const int meshesCapacity, const  int texturesCapacity, const  int shadersCapacity);

	void AddMesh(const Mesh& mesh);

	Mesh& CreateMesh(const char* path);
	Mesh & CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
	Texture& CreateTexture(const char* path,
		Texture::Filtering filtering = Texture::Nearest,
		Texture::EdgeMode edgeMode = Texture::Wrap);
	Shader& CreateShader(const char* path);
	void RebuildAll();
	void Dispose();
};