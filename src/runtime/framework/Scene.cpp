#include "Scene.h"

#include "runtime/log/Log.h"
#include "runtime/framework/Node.h"

#include <cassert>

namespace shzk
{
	void Scene::Tick()
	{
		for (auto& node : m_nodes)
		{
			
		}
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

	void Scene::AddNode(std::shared_ptr<Node> node)
	{
		assert(node);
		m_nodes.push_back(node);
		SHZK_LOG_INFO("Node {} added to Scene {}", node->m_name, m_name);
	}

	void Scene::AddCameraNode(std::shared_ptr<Node> node)
	{
		assert(node);
		for (auto& component : node->m_components)
		{
			if (component->GetType() == ComponentType::Camera)
			{
				m_cameraNode = node;
				SHZK_LOG_INFO("Camera Node {} added to Scene {}", node->m_name, m_name);
				return;
			}
		}
		SHZK_LOG_ERROR("Node {} doesn't have Camera Component, cannot be added as Camera Node to Scene {}", node->m_name, m_name);
	}
}
