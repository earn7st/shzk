#pragma once

#include <memory>

namespace shzk
{
	class Node;

	enum class ComponentType
	{	
		Unknown				= 0,
		Transform			= 1,
		Mesh				= 2,
		Camera				= 3,
		DirectionalLight	= 4,
		PointLight			= 5,
	};

	class Component
	{
	public:
		Component(ComponentType type) : m_type(type) {}
		virtual ~Component() = default;

		ComponentType GetType() const { return m_type; }

	protected:
		std::weak_ptr<Node> m_owner;
		ComponentType m_type;
	
		friend class Node;
		friend class Scene;
	};
}