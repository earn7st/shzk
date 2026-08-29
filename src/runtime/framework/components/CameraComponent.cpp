#include "CameraComponent.h"
#include "TransformComponent.h"

#include "runtime/core/Math.h"
#include "runtime/log/Log.h"
#include "runtime/global/Engine.h"
#include "runtime/input/InputSystem.h"
#include "runtime/framework/Node.h"

#include <memory>
#include <glm/glm.hpp>


namespace shzk
{
	void CameraComponent::Tick(float dt)
	{
		if (m_bIsActiveCamera) ProcessInputMovement(dt);
	}

	glm::mat4x4 CameraComponent::GetViewMatrix() const
	{
		auto node = m_owner.lock();
		auto transformComp = node->TryGetComponent<TransformComponent>();
		if (!transformComp)
		{
			SHZK_LOG_ERROR("Camera doesn't have a transform!");
			assert(false);
			return glm::mat4x4{};
		}

		glm::vec3 position	= transformComp->GetPosition();
		glm::vec3 front		= transformComp->Front();
		glm::vec3 up		= transformComp->Up();

		return glm::lookAtRH(position, position + front, up);
	}

	glm::mat4x4 CameraComponent::GetProjMatrix(float aspect) const
	{
		glm::mat4 result = glm::perspectiveRH_ZO(glm::radians(m_fovY), aspect, m_nearPlane, m_farPlane);
		result[1][1] *= -1;
		return result;
	}

	// --- private functions ---
	// Movement
	void CameraComponent::ProcessInputMovement(float dt)
	{
		std::shared_ptr<TransformComponent> transformComp = m_owner.lock()->TryGetComponent<TransformComponent>();
		if (!transformComp) return;

		// movement
		float delta = m_speed * dt / 2.0f;
		auto& inputSystem = Engine::GetInputSystem();
		glm::vec3 deltaPosition = glm::vec3(0.f);
		if (inputSystem->IsKeyDown(KeyCode::LeftShift))		delta *= 5;
		if (inputSystem->IsKeyDown(KeyCode::W))				deltaPosition += transformComp->Front() * delta;
		if (inputSystem->IsKeyDown(KeyCode::S))				deltaPosition -= transformComp->Front() * delta;
		if (inputSystem->IsKeyDown(KeyCode::A))				deltaPosition -= transformComp->Right() * delta;
		if (inputSystem->IsKeyDown(KeyCode::D))				deltaPosition += transformComp->Right() * delta;
		if (inputSystem->IsKeyDown(KeyCode::Q))				deltaPosition += transformComp->Up() * delta;
		if (inputSystem->IsKeyDown(KeyCode::E))				deltaPosition -= transformComp->Up() * delta;
		transformComp->Translate(deltaPosition);

		if (inputSystem->IsMouseButtonDown(MouseButton::Right))
		{
			glm::vec2 offset = inputSystem->GetMouseDelta() * m_sens;

			float yawDelta = -offset.x;
			float pitchDelta = -offset.y;
			float clampedDelta = Math::ClampPitch(transformComp->GetEulerDegree().x, pitchDelta);

			glm::quat rot = transformComp->GetRotation();

			glm::quat yawQ = glm::angleAxis(glm::radians(yawDelta), glm::vec3(0.f, 1.f, 0.f));
			glm::quat pitchQ = glm::angleAxis(glm::radians(clampedDelta), glm::vec3(1.f, 0.f, 0.f));

			transformComp->SetRotation(yawQ * rot * pitchQ);
		}

		m_fovY -= inputSystem->GetScrollDelta().y * m_scrollSens * 2;
		m_fovY = m_fovY > m_maxFovY ? m_maxFovY : m_fovY;
		m_fovY = m_fovY < m_minFovY ? m_minFovY: m_fovY;
	}

}