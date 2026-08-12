#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace shzk
{
	typedef struct Transform
	{
	public:
		glm::vec3 translation	= { 0.f, 0.f, 0.f };
		glm::quat rotation		= glm::quat(1.0f, 0.0f, 0.0f, 0.0f);  // identity
		glm::vec3 scale			= { 1.f, 1.f, 1.f};

		glm::mat4x4 ToMat4x4() const
		{
			glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
			glm::mat4 R = glm::mat4_cast(rotation);
			glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
			return T * R * S;
		}
	} Transform;
}