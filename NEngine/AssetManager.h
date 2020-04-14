#pragma once
#include "Texture.h"
#include <optional>
struct Mesh;
class Shader;

class AssetManager
{
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

	std::vector<std::string> shaderPaths;
	std::vector<std::string> textureNames;
	std::vector<std::string> meshNames;

public:
	std::vector<Shader> shaders;

	AssetManager(const int meshesCapacity, const  int texturesCapacity, const  int shadersCapacity);

	void LoadAll(const std::string& folderPath);

	int GetMeshIndex(const std::string& name);
	Mesh& GetMesh(int i);
	std::optional<std::reference_wrapper<Mesh>> GetMesh(const std::string& name);

	void AddMesh(const Mesh& mesh, const char* name);
	Mesh& CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const char* name);
	Mesh& CreateMesh(const char* path);

	std::optional<std::reference_wrapper<Texture>> GetTexture(const char* name);
	Texture& CreateTexture(const char* path,
		Texture::Filtering filtering = Texture::Nearest,
		Texture::EdgeMode edgeMode = Texture::Wrap);

	std::optional<std::reference_wrapper<Shader>> GetShader(const std::string& name);
	Shader& CreateShader(const char* path);

	void RebuildAll();
	void ReloadShaders();
	void Dispose();
};