#pragma once

#include "Component.h"
#include "runtime/core/Transform.h"

namespace shzk
{
	class TransformComponent : public Component
	{
	public:
		TransformComponent() : Component(ComponentType::Transform) {}
		~TransformComponent() = default;

		void SetTransform(const Transform& transform)		{ m_transform = transform; }
		void SetTranslation(const glm::vec3& translation)	{ m_transform.SetTranslation(translation); }
		void SetRotation(const glm::quat& rotation)			{ m_transform.SetRotation(rotation); }
		void SetScale(const glm::vec3& scale)				{ m_transform.SetScale(scale); }

		const Transform& GetTransform() const { return m_transform; }

		void Translate(const glm::vec3& translation)	{ m_transform.Translate(translation); }
		void Rotate(const glm::quat& rotation)			{ m_transform.Rotate(rotation); }
		void Scale(const glm::vec3& scale)				{ m_transform.Scale(scale); }

		glm::vec3 Front() const { }


	private:
		Transform m_transform;
	};
}