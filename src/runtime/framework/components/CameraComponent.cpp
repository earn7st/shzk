#include "CameraComponent.h"
#include "TransformComponent.h"

#include "runtime/log/Log.h"
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
}