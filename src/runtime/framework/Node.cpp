#include "Node.h"

#include <cassert>

namespace shzk
{
	void Node::Tick()
	{
	}

	void Node::AddChildren(std::shared_ptr<Node> node) 
	{
		assert(node);
		node->m_parent = shared_from_this();
		m_children.push_back(node);
	}

	void Node::AddComponent(std::shared_ptr<Component> component)
	{
		assert(component);
		component->m_owner = shared_from_this();
		m_components.push_back(component);
	}
}
