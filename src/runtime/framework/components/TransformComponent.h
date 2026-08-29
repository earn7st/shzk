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

		void SetTranslation(const glm::vec3& position)		{ m_transform.SetTranslation(position); }
		void SetRotation(const glm::quat& quat)				{ m_transform.SetRotation(quat); }
		void SetEulerDegree(const glm::vec3& eulerDegree)	{ m_transform.SetEulerDegree(eulerDegree); }
		void SetScale(const glm::vec3& scale)				{ m_transform.SetScale(scale); }

		const Transform& GetTransform() const { return m_transform; }		
		glm::vec3 GetPosition() const { return m_transform.GetPosition();}
		glm::quat GetRotation() const { return m_transform.GetRotation(); }
		glm::vec3 GetEulerDegree() const { return m_transform.GetEulerDegree(); }
		glm::vec3 GetScale() const { return m_transform.GetScale(); }

		void Translate(const glm::vec3& translation)	{ m_transform.Translate(translation); }
		void Rotate(const glm::vec3& eulerDegree)		{ m_transform.Rotate(eulerDegree); }
		void Scale(const glm::vec3& scale)				{ m_transform.Scale(scale); }

		glm::vec3 Front() const { return m_transform.Front(); }
		glm::vec3 Up() const { return m_transform.Up(); }
		glm::vec3 Right() const { return m_transform.Right(); }

	private:
		Transform m_transform;
	};
}