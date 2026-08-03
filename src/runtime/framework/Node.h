#pragma once

#include "runtime/framework/components/Component.h"

#include <memory>
#include <string>
#include <vector>

namespace shzk
{
	class Component;

	class Node : public std::enable_shared_from_this<Node>
	{
	public:
		Node() = default;
		~Node() = default;

		void Tick();

		uint32_t GetId() const { return m_id; }
		const std::string& GetName() const { return m_name; }

		void AddChildren(std::shared_ptr<Node> node);
		void AddComponent(std::shared_ptr<Component> component);

	private:
		uint32_t	m_id;
		std::string m_name;

		std::weak_ptr<Node> m_parent;
		std::vector<std::shared_ptr<Node>> m_children;

		std::vector<std::shared_ptr<Component>> m_components;

		friend class Scene;
	};
}