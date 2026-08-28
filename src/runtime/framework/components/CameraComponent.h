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
		void SetIsActiveCamera(bool t) { m_bIsActiveCamera = t; }

		CameraProjectionMode GetCameraProjectionMode() const { return m_mode; }
		float GetFovY() const { return m_fovY; }
		float GetOrthoHeight() const { return m_orthoHeight; }
		float GetNearPlane() const { return m_nearPlane; }
		float GetFarPlane() const { return m_farPlane; }
		bool IsActiveCamera() const { return m_bIsActiveCamera; }

		// TODO: Multi threaded, compute during scene tick
		glm::mat4x4 GetViewMatrix(); // need to know transform info (from transform component)
		glm::mat4x4 GetProjectionMatrix(float aspect = 0);
		
		virtual void Tick(float dt) override final;

	private:
		CameraProjectionMode m_mode = CameraProjectionMode::Perspective;

		// ortho
		float m_orthoHeight = 10.f;
		// perspective
		float m_fovY		= 60.0f;	
		float m_nearPlane	= 0.1f;
		float m_farPlane	= 100.0f;

		float m_maxFovY		= 135.f;
		float m_minFovY		= 30.f;

		bool m_bIsActiveCamera = false;

		// Movement
		void ProcessInputMovement(float dt);	// TODO: Camera Controller
		float m_speed		= 5.f;
		float m_sens		= 1.f;
		float m_scrollSens	= 1.f;
	};
}