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
		Node(uint32_t id, std::string name) : m_id(id), m_name(name) {}
		~Node() = default;

		void Tick(float dt);

		void SetId(uint32_t id) { m_id = id; }
		void SetName(std::string name) { m_name = std::move(name); }
		uint32_t GetId() const { return m_id; }
		const std::string& GetName() const { return m_name; }
		
		const std::vector<std::shared_ptr<Node>>& GetChildren() const { return m_children; }
		void AddChildren(const std::shared_ptr<Node>& node);
		void AddComponent(const std::shared_ptr<Component>& component);

		template <typename T>
		std::shared_ptr<T> TryGetComponent()
		{
			for (auto& component : m_components)
			{
				std::shared_ptr<T> cast = std::dynamic_pointer_cast<T>(component);
				if (cast != nullptr) return cast;
			}
			return nullptr;
		}

		template<typename T>
		std::shared_ptr<T> TryGetComponentInParent(bool self = false)
		{
			if (self)
			{
				std::shared_ptr<T> component = TryGetComponent<T>();
				if (component) return component;
			}

			std::shared_ptr<Node> node = m_parent.lock();
			if (node) return node->TryGetComponentInParent<T>(true);

			return nullptr;
		}

	private:
		uint32_t	m_id = -1;
		std::string m_name;

		std::weak_ptr<Node> m_parent;
		std::vector<std::shared_ptr<Node>> m_children;

		std::vector<std::shared_ptr<Component>> m_components;

		friend class Scene;
	};
}