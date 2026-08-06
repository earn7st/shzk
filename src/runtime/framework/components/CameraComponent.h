#pragma once

#include "COmponent.h"

namespace shzk
{
	class CameraComponent : public Component
	{
	public:
		CameraComponent() : Component(ComponentType::Camera) {}
		~CameraComponent() = default;

	private:
		float fov			= 60.0f;	
		float nearPlane		= 0.1f;
		float farPlane		= 100.0f;
	};
}