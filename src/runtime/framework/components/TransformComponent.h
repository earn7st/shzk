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

		void SetTransform(const Transform& transform) { m_transform = transform; }
		const Transform& GetTransform() const { return m_transform; }

	private:
		Transform m_transform;
	};
}