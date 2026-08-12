#include "Scene.h"

#include "runtime/log/Log.h"
#include "runtime/framework/Node.h"
#include "runtime/framework/components/CameraComponent.h"

#include <cassert>

namespace shzk
{
	void Scene::Tick()
	{
		for (auto& node : m_nodes)
		{
			
		}
	}

	void Scene::Clear()
	{
		m_nodes.clear();
	}

	std::shared_ptr<Node> Scene::GetNodeByName(const std::string& name) const
	{
		for (auto& node : m_nodes)
		{
			if (node->m_name == name)
			{
				return node;
			}
		}
		SHZK_LOG_WARN("Scene {} doesn't have Node which name is {}", m_name, name);
		return std::shared_ptr<Node>();
	}

	std::shared_ptr<Node> Scene::GetNodeById(uint32_t id) const
	{
		for (auto& node : m_nodes)
		{
			if (node->m_id == id)
			{
				return node;
			}
		}
		SHZK_LOG_WARN("Scene {} doesn't have Node which id is {}", m_name, id);
		return std::shared_ptr<Node>();
	}

	std::shared_ptr<CameraComponent> Scene::GetActiveCamera()
	{
		for (auto& node : m_nodes)
		{
			std::shared_ptr<CameraComponent> component = node->TryGetComponent<CameraComponent>();
			if (component) return component;
		}
		return nullptr;
	}

	void Scene::AddNode(std::shared_ptr<Node> node)
	{
		assert(node);
		m_nodes.push_back(node);
		SHZK_LOG_INFO("Node {} added to Scene {}", node->m_name, m_name);
	}
}
