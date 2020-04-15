#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "AssetManager.h"
#include "ModelReader.h"
#include <filesystem>

#define AM_LOG 0;

AssetManager::AssetManager(
	const int meshesCapacity,
	const int texturesCapacity,
	const int shadersCapacity,
	const int audioClipsCapacity)
{
	meshes.reserve(meshesCapacity);
	meshNames.reserve(meshesCapacity);

	textures.reserve(texturesCapacity);

	shaders.reserve(shadersCapacity);
	shaderPaths.reserve(shadersCapacity);

	clips.reserve(audioClipsCapacity);
	clipNames.reserve(audioClipsCapacity);
}


void AssetManager::LoadAll(const std::string & folderPath)
{
	std::cout << std::endl << "AssetManager:: LOADING ASSETS" << std::endl;
	LoadRecursively(folderPath);
	std::cout << "AssetManager:: Loading assets completed" << std::endl << std::endl;
}

void AssetManager::LoadRecursively(const std::string& folderPath)
{
	for (const auto & entry : std::filesystem::directory_iterator(folderPath))
	{
		if (entry.is_directory())
		{
			LoadRecursively(entry.path().string());
			continue;
		}

		auto ext = entry.path().extension();

		if (ext == ".ply")
		{
			CreateMesh(entry.path().string().c_str());
		}
		else if (ext == ".png")
		{
			CreateTexture(entry.path().string().c_str());
		}
		else if (ext == ".glsl")
		{
			CreateShader(entry.path().string().c_str());
		}
		else if (ext == ".wav" || ext == ".ogg")
		{
			CreateWav(entry.path().string().c_str());
		}
	}
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

std::optional<std::reference_wrapper<Mesh>> AssetManager::GetMesh(const std::string& name)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		if (meshNames[i] == name)
			return meshes[i];
	}

	return {};
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

#if AM_LOG
	std::cout << "AssetManager:: Loaded Mesh: " << name << std::endl;
#endif

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

#if AM_LOG
	std::cout << "AssetManager:: Loaded texture: " << name << std::endl;
#endif

	return textures[textures.size() - 1];
}

std::optional<std::reference_wrapper<Shader>> AssetManager::GetShader(const std::string & name)
{
	for (size_t i = 0; i < shaders.size(); i++)
	{
		std::filesystem::path path(shaderPaths[i]);
		if (path.stem() == name)
			return shaders[i];
	}
	return {};
}

Shader & AssetManager::CreateShader(const char * path)
{
	ShaderSource source = ShaderReader::Parse(path);
	Shader shader(source);
	shaders.push_back(source);
	shaderPaths.push_back(path);

	return shaders[shaders.size() - 1];
}

std::optional<std::reference_wrapper<SoLoud::Wav>> AssetManager::GetAudioClip(const std::string & name)
{
	for (size_t i = 0; i < clips.size(); i++)
	{
		if (clipNames[i] == name)
			return clips[i];
	}

	std::cout << "ERROR: AssetManager:: " << name << " doesn't exist";
	return {};
}

SoLoud::Wav& AssetManager::CreateWav(const std::string & path)
{
	SoLoud::Wav& wav = clips.emplace_back();
	wav.load(path.c_str());

	std::filesystem::path filepath(path);
	auto name = filepath.stem().string();
	clipNames.push_back(name);

#if AM_LOG
	std::cout << "AssetManager:: Loaded audio clip: " << name << std::endl;
#endif
	return wav;
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