#pragma once
#include <optional>

#include "Texture.h"
//#include "soloud.h"
#include "soloud_wav.h"

struct Mesh;
class Shader;

class AssetManager
{
private:
	std::vector<Mesh> meshes;
	std::vector<Texture> textures;
	std::vector<Shader> shaders;
	std::vector<SoLoud::Wav> clips;

	std::vector<std::string> shaderPaths;
	std::vector<std::string> textureNames;
	std::vector<std::string> meshNames;
	std::vector<std::string> clipNames;

public:

	AssetManager(
		const int meshesCapacity,
		const int texturesCapacity,
		const int shadersCapacity,
		const int audioClipsCapacity = 16);

	void LoadAll(const std::string& folderPath);

	// Mesh
	int GetMeshIndex(const std::string& name);
	Mesh& GetMesh(int i);
	std::optional<std::reference_wrapper<Mesh>> GetMesh(const std::string& name);

	void AddMesh(const Mesh& mesh, const char* name);
	Mesh& CreateMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, const char* name);
	Mesh& CreateMesh(const char* path);

	// Texture
	std::optional<std::reference_wrapper<Texture>> GetTexture(const char* name);
	Texture& CreateTexture(const char* path,
		Texture::Filtering filtering = Texture::Nearest,
		Texture::EdgeMode edgeMode = Texture::Wrap);

	// Shader
	std::optional<std::reference_wrapper<Shader>> GetShader(const std::string& name);
	Shader& CreateShader(const char* path);

	// Audio clips
	std::optional<std::reference_wrapper<SoLoud::Wav>> GetAudioClip(const std::string& name);
	SoLoud::Wav& CreateWav(const std::string& path);

	void RebuildAll();
	void ReloadShaders();
	void Dispose();
};