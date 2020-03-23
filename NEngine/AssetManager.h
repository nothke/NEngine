#pragma once
struct Mesh;
class Texture;
class Shader;

class AssetManager
{
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;

public:
	std::vector<Shader> shaders;

	AssetManager(const int meshesCapacity, const  int texturesCapacity, const  int shadersCapacity);

	Mesh& CreateMesh(const char* path);
	Texture& CreateTexture(const char* path);
	Shader& CreateShader(const char* path);
	void RebuildAll();
	void Dispose();
};