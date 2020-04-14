#pragma once
#include <soloud.h>
//#include <soloud_wav.h>
#include <iostream>

#include <glm/vec3.hpp>
#include "AssetManager.h"

using namespace glm;

struct Scene
{
	AssetManager& assets;
	SoLoud::Soloud& audio;

	Scene(AssetManager& assets, SoLoud::Soloud& audio)
		: assets(assets), audio(audio) {}

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