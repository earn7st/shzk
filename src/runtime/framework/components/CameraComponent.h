#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace shzk
{
	class CameraComponent : public Component
	{
	public:
		CameraComponent() : Component(ComponentType::Camera) {}
		~CameraComponent() = default;

	private:
		float m_fov			= 60.0f;	
		float m_nearPlane		= 0.1f;
		float m_farPlane		= 100.0f;

		glm::vec3 m_front{ -1.f, 0.f, 0.f};
	};
}