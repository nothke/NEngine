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