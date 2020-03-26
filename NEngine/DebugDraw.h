#pragma once

#include "AssetManager.h"
#include "glm/glm.hpp"
#include "GLAssert.h"
using namespace glm;

namespace DebugDraw
{
	std::vector<vec3> points;

	void Init(int pointsCapacity = 64)
	{
		points.reserve(64);
	}

	void Line(vec3 start, vec3 end)
	{
		points.push_back(start);
		points.push_back(end);
	}

	void Render()
	{
		if (points.size() == 0)
			return;

		glClear(GL_DEPTH_BUFFER_BIT);

		glBegin(GL_LINES);

		for (vec3& p : points)
		{
			glVertex3f(p.x, p.y, p.z);
		}

		glEnd();

		points.clear();
	}
}