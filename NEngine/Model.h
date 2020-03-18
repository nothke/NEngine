#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "Mesh.h"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

struct Model
{
	vec3 position;
	quat rotation = quat();
	vec3 scale = vec3(1);

	Mesh& mesh;

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
		: mesh(mesh), position(position) {}

	Model(vec3 position, quat rotation, vec3 scale, Mesh&mesh)
		: mesh(mesh), position(position), rotation(rotation), scale(scale) {}

	Model(Mesh&mesh)
		: mesh(mesh) {}

	inline mat4 LocalToWorld()
	{
		if (isDirty)
		{
			localToWorld = mat4(1);
			localToWorld = glm::translate(localToWorld, position);
			localToWorld = localToWorld * mat4_cast(rotation);
			localToWorld = glm::scale(localToWorld, scale);

			isDirty = false;
		}

		return localToWorld;
	}

private:
	glm::mat4 localToWorld;
};