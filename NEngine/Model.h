#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "Mesh.h"
#include "glm/gtc/quaternion.hpp"
#include "FrustumCull.h"
#include "Texture.h"

using namespace glm;

struct Model
{
	vec3 position;
	quat rotation = quat();
	vec3 scale = vec3(1);

	Mesh& mesh;
	Texture* texture = nullptr;

	bool isDirty = true;

	void SetPosition(vec3 position) { this->position = position; isDirty = true; }
	void SetRotation(quat rotation) { this->rotation = rotation; isDirty = true; }
	void SetScale(float scale) { this->scale = vec3(scale); isDirty = true; }
	void SetScale(vec3 scale) { this->scale = scale; isDirty = true; }

	void SetRotation(vec3 eulerAngles)
	{
		rotation = quat(vec3(eulerAngles.x, eulerAngles.y, eulerAngles.z));
		isDirty = true;
	}

	Model(vec3 position, Mesh&mesh)
		: mesh(mesh), position(position) {
		UpdateModelMatrix();
	}

	Model(vec3 position, Mesh&mesh, Texture& texture)
		: mesh(mesh), position(position), texture(&texture) {
		//std::cout << "Assigned texture " << std::endl;
		UpdateModelMatrix();
	}

	Model(vec3 position, quat rotation, vec3 scale, Mesh&mesh)
		: mesh(mesh), position(position), rotation(rotation), scale(scale) {
		UpdateModelMatrix();
	}

	Model(Mesh&mesh)
		: mesh(mesh) {
		UpdateModelMatrix();
	}

	bool IsVisible(const Frustum& frustum)
	{
		return frustum.IsBoxVisible(boundsMin, boundsMax);
	}

	inline void UpdateModelMatrix()
	{
		localToWorld = mat4(1);
		localToWorld = glm::translate(localToWorld, position);
		localToWorld = localToWorld * mat4_cast(rotation);
		localToWorld = glm::scale(localToWorld, scale);

		// TODO: 1000 is temp, make method to find bounds
		boundsMin = position + mesh.boundsMin;
		boundsMax = position + mesh.boundsMax;

		isDirty = false;
	}

	inline mat4& LocalToWorld()
	{
		if (isDirty)
		{
			UpdateModelMatrix();
		}

		return localToWorld;
	}

private:
	mat4 localToWorld;

	vec3 boundsMin;
	vec3 boundsMax;
};