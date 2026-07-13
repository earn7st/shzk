#pragma once

#include "Component.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace vkR
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent() = default;
		~TransformComponent() = default;

		void Initialize();
		virtual void Tick() final;

	private:
		glm::vec3 m_position;
		glm::quat m_rotation;
		glm::vec3 m_scale;
	};
}