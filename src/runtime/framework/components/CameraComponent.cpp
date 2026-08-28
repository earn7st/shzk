#include "CameraComponent.h"
#include "TransformComponent.h"

#include "runtime/log/Log.h"
#include "runtime/global/Engine.h"
#include "runtime/input/InputSystem.h"
#include "runtime/framework/Node.h"

#include <memory>
#include <glm/glm.hpp>

namespace shzk
{
	glm::mat4x4 CameraComponent::GetViewMatrix()
	{
		std::shared_ptr<Node> node = m_owner.lock();
		std::shared_ptr<TransformComponent> transformComp = node->TryGetComponent<TransformComponent>();
		if (!transformComp) 
		{
			SHZK_LOG_ERROR("Active camera doesn't have a transform!");
			assert(false);
			return glm::identity<glm::mat4x4>();
		}
		return glm::inverse(transformComp->GetTransform().ToMat4x4());
	}

	glm::mat4x4 CameraComponent::GetProjectionMatrix(float aspect)
	{
		switch (m_mode)
		{
		case CameraProjectionMode::Perspective: 
		{
			glm::mat4x4 proj = glm::perspective(glm::radians(m_fovY), aspect, m_nearPlane, m_farPlane);
			// proj[1][1] *= -1.0f;	// vertical flip
			return proj;
		}
		case CameraProjectionMode::Orthographic:
		{
			float halfHeight = m_orthoHeight * 0.5f;
			float halfWidth = halfHeight * aspect;
			glm::mat4x4 proj = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
			break;
		}
		default: SHZK_LOG_ERROR("Unsupported CameraProjectionMode!"); break;
		}
		return glm::identity<glm::mat4x4>();
	}

	void CameraComponent::Tick(float dt)
	{
		if (m_bIsActiveCamera)
		{
			ProcessInputMovement(dt);
		}
	}

	// --- private functions ---
	// Movement
	void CameraComponent::ProcessInputMovement(float dt)
	{
		std::shared_ptr<TransformComponent> transformComponent = m_owner.lock()->TryGetComponent<TransformComponent>();
		if (!transformComponent) return;

		float delta = m_speed * dt / 1000.0f;

		auto& inputSystem = Engine::GetInputSystem();
		glm::vec3 deltaPosition = glm::vec3(0.f);
		if (inputSystem->IsKeyPressed(KeyCode::LeftShift))		delta *= 5;
		if (inputSystem->IsKeyPressed(KeyCode::W))				deltaPosition += transformComponent->Front() * delta;
		if (inputSystem->IsKeyPressed(KeyCode::S))				deltaPosition -= transformComponent->Front() * delta;
		if (inputSystem->IsKeyPressed(KeyCode::A))				deltaPosition -= transformComponent->Right() * delta;
		if (inputSystem->IsKeyPressed(KeyCode::D))				deltaPosition += transformComponent->Right() * delta;
		if (inputSystem->IsKeyPressed(KeyCode::Q))				deltaPosition += transformComponent->Up() * delta;
		if (inputSystem->IsKeyPressed(KeyCode::E))				deltaPosition -= transformComponent->Up() * delta;
		transformComponent->Translate(deltaPosition);

		if (inputSystem->IsMouseButtonPressed(MouseButton::Right))
		{
			glm::vec2 offset = - inputSystem->GetMouseDelta() * m_sens;

			glm::vec3 eulerAngle = transformComponent->GetEulerAngle();
			eulerAngle = Math::ClampEulerAngle(eulerAngle + glm::vec3(0.f, offset.x(), offset.y()));
			transformComponent->SetRotation(eulerAngle);

			m_fovY -= inputSystem->GetScrollDelta().y() * m_scrollSens * 2;
			m_fovY = m_fovY > m_maxFovY ? m_maxFovY : m_fovY;
			m_fovY = m_fovY < m_minFovY ? m_minFovY: m_fovY;
		}
	}

}