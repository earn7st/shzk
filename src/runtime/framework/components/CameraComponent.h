#pragma once

#include "Component.h"

#include <glm/glm.hpp>

namespace shzk
{
	enum class CameraProjectionMode : uint8_t
	{
		Perspective = 0,
		Orthographic = 1,

		Max,
	};

	class CameraComponent : public Component
	{
	public:
		CameraComponent() : Component(ComponentType::Camera) {}
		~CameraComponent() = default;

		void SetFovY(float fovY) { m_fovY = fovY; }
		float SetOrthoHeight(float orthoHeight) { m_orthoHeight = orthoHeight; }
		void SetNearPlane(float np) { m_nearPlane = np; }
		void SetFarPlane(float fp) { m_farPlane = fp; }

		CameraProjectionMode GetCameraProjectionMode() const { return m_mode; }
		float GetFovY() const { return m_fovY; }
		float GetOrthoHeight() const { return m_orthoHeight; }
		float GetNearPlane() const { return m_nearPlane; }
		float GetFarPlane() const { return m_farPlane; }

		glm::mat4x4 GetViewMatrix(); // need to know transform info (from transform component)
		glm::mat4x4 GetProjectionMatrix(float aspect = 0);

	private:
		CameraProjectionMode m_mode = CameraProjectionMode::Perspective;

		// ortho
		float m_orthoHeight = 10.f;
		// perspective
		float m_fovY		= 60.0f;	
		float m_nearPlane	= 0.1f;
		float m_farPlane	= 100.0f;

	};
}