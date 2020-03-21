#pragma once
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
using namespace glm;

class Camera
{
public:
	vec3 position;
	quat rotation;

	mat4 p;
	mat4 v;
	mat4 vp;

	vec3 forward;
	vec3 right;

	static constexpr float nearPlane = 0.1f;
	static constexpr float farPlane = 10000.0f;

	void SetProjection(float verticalFovDegrees, float aspectRatio)
	{
		p = glm::perspective(glm::radians(verticalFovDegrees), aspectRatio,
			0.1f, 1000.0f);
	}

	void SetInputRotation(vec2 input)
	{
		rotation =
			quat(vec3(input.y, 0, 0)) *
			quat(vec3(0, input.x, 0));
	}

	void MoveRelative(vec2 by)
	{
		position += forward * by.y + right * by.x;
	}

	void UpdateRotation()
	{
		v = glm::mat4(1.0f);
		v = v * mat4_cast(rotation);

		// camera forward and right
		const glm::mat4 inv = glm::inverse(v);
		right = -glm::normalize(inv[0]);
		forward = -glm::normalize(inv[2]);

		// Constrain to horizontal plane (walking):
		//forward.y = 0;
		//forward = glm::normalize(forward);
	}

	void Update()
	{
		v = translate(v, position);
		vp = p * v;
	}

	Camera()
	{
		position = vec3(0);
		rotation = quat();
	}
};

