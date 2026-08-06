#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace shzk
{
	typedef struct Transform
	{
		glm::vec3 translation	= { 0.f, 0.f, 0.f };
		glm::quat rotation		= glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // identity
		glm::vec3 scale			= { 1.f, 1.f, 1.f};
	} Transform;
}