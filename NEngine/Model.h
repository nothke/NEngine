#pragma once

#include "glm/gtc/matrix_transform.hpp"
#include "Mesh.h"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

struct Model
{
	vec3 position;
	quat rotation;
	vec3 scale;

	Mesh& mesh;

	bool isDirty = true;

	void SetPosition(vec3 position) { this->position = position; isDirty = true; }
	void SetRotation(quat rotation) { this->rotation = rotation; isDirty = true; }
	void SetRotation(vec3 scale) { this->scale = scale; isDirty = true; }

	Model(vec3 position, Mesh&mesh)
		: mesh(mesh), position(position) {}

	Model(vec3 position, quat rotation, vec3 scale, Mesh&mesh)
		: mesh(mesh), position(position), rotation(rotation), scale(scale) {}

	Model(Mesh&mesh)
		: mesh(mesh) {}

	mat4 LocalToWorld()
	{
		if (isDirty)
		{
			localToWorld = glm::translate(glm::mat4(1), position);
			//model = glm::rotate(model, )
			isDirty = false;
		}

		return localToWorld;
	}

private:
	glm::mat4 localToWorld;
};