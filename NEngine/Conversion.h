#pragma once
#include <imgui/imgui.h>
#include <glm/glm.hpp>

glm::vec4 from(ImVec4 v)
{
	return { v.x, v.y, v.z, v.w };
}

ImVec4 from(glm::vec4 v)
{
	return { v.x, v.y, v.z, v.w };
}

#include "LinearMath/btVector3.h"

glm::vec3 from(btVector3 v)
{
	return { v.getX(), v.getY(), v.getZ() };
}

btVector3 from(glm::vec3 v)
{
	return btVector3(v.x, v.y, v.z);
}

#include "LinearMath/btQuaternion.h"

glm::quat from(btQuaternion q)
{
	return { q.getX(), q.getY(), q.getZ(), q.getW() };
}