#pragma once
#include <soloud.h>
//#include <soloud_wav.h>
#include <iostream>

#include <glm/vec3.hpp>
#include "AssetManager.h"
#include <parser.h>
#include "Physics.h"

using namespace glm;

struct ParsedModel
{
	std::string name;
	std::string texture;
	vec3 pos;
	vec3 rot;
	vec3 scl;
	bool createCollider;
};

struct Scene
{
	AssetManager& assets;
	SoLoud::Soloud& audio;
	Physics& physics;

	std::vector<Model> models;

	Scene(AssetManager& assets, SoLoud::Soloud& audio, Physics& physics)
		: assets(assets), audio(audio), physics(physics)
	{
		models.reserve(1024);
	}

	void LoadFromCSV(const std::string& path)
	{
		std::ifstream f(path);
		aria::csv::CsvParser parser = aria::csv::CsvParser(f);

		std::vector<ParsedModel> parsedModels;
		for (auto& row : parser)
		{
			ParsedModel m;

			m.name = row[0];
			m.texture = row[1];
			m.pos = { stof(row[2]), stof(row[3]), stof(row[4]) };
			m.rot = { stof(row[5]), stof(row[6]), stof(row[7]) };
			m.scl = { stof(row[8]), stof(row[9]), stof(row[10]) };
			m.createCollider = stoi(row[11]);

			parsedModels.push_back(m);
		}

		std::map<std::string, btCollisionShape*> shapeMap;

		// Add models from CSV
		for (auto& m : parsedModels)
		{
			int i = assets.GetMeshIndex(m.name);
			if (i >= 0)
			{
				vec3 pos = m.pos;
				pos.x = -m.pos.x;

				Model model = Model(pos, assets.GetMesh(i));
				model.SetRotation(vec3(radians(m.rot.x), radians(-m.rot.y), radians(m.rot.z)));

				model.SetScale(m.scl);
				auto opt_tex = assets.GetTexture(m.texture.c_str());
				if (opt_tex.has_value())
					model.texture = &opt_tex.value().get();
				models.push_back(model);

				if (m.createCollider)
				{
					mat4 matrix = model.LocalToWorld();

					if (shapeMap.count(m.name) > 0)
					{
						physics.CreateBody(shapeMap[m.name], 0, (btScalar*)&matrix[0]);
					}
					else
					{
						auto mcol = physics.CreateMeshCollider(model.mesh);
						shapeMap.emplace(m.name, mcol);
						physics.CreateBody(mcol, 0, (btScalar*)&matrix[0]);
						std::cout << "Creating collider for " << m.name << std::endl;
					}
				}
			}
		}
	}

	Model& CreateModel(Mesh& mesh, Texture& texture)
	{
		return models.emplace_back(vec3(0), mesh, texture);
		//return models[models.size() - 1];
	}

	SoLoud::handle PlayAudioSource(
		const std::string& name,
		vec3 position,
		float volume = 1,
		float rollofFactor = 1,
		bool looping = false)
	{
		auto opt = assets.GetAudioClip(name);

		if (!opt.has_value()) // return empty
		{
			std::cout << "ERROR: Scene:: Clip with name " << name << " not found!" << std::endl;
			return SoLoud::handle();
		}

		auto& clip = opt.value().get();
		auto handle = audio.play3d(clip, -position.x, -position.y, -position.z);
		audio.set3dSourceAttenuation(handle, 1, rollofFactor);
		audio.setLooping(handle, looping);
		return handle;
	}
};