#pragma once

namespace shzk
{
	class CameraComponent : public Component
	{
	public:


	private:
		float nearPlane		= 0.1f;
		float farPlane		= 100.0f;
	};
}