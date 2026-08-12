#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace shzk
{
	struct DirectionalLightData
	{
		float intensity = 1.f;
		glm::vec3 direction{1.f, 1.f, 1.f};
	}; 

	class DirectionalLightComponent : public Component
	{
	public:
		DirectionalLightComponent() : Component(ComponentType::DirectionalLight) {}
		~DirectionalLightComponent() = default;

	private:
		DirectionalLightData m_data;
	};
}