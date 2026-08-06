#pragma once

#include "runtime/framework/Node.h"

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
		~Component() = default;

		virtual void Init();

		ComponentType GetType() const { return m_type; }

	protected:
		std::weak_ptr<Node> m_owner;
		ComponentType m_type;
	
		friend class Node;
	};
}