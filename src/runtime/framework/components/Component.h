#pragma once

#include "runtime/framework/Node.h"

#include <memory>

namespace shzk
{
	class Node;

	enum class ComponentType
	{	
		Undefined	= 0,
		Transform	= 1,
		Mesh		= 2,
		Camera		= 3,
		Light		= 4,
	};

	class Component
	{
	public:
		Component() = default;
		~Component() = default;

		virtual void Init();

		ComponentType GetType() const { return m_type; }

	protected:
		std::weak_ptr<Node> m_Node;
		ComponentType m_type;
	
		friend class Node;
	};
}