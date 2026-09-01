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
		SkyBox				= 4,
		DirectionalLight	= 5,
		PointLight			= 6,
		SkyLight			= 7,

		Max,
	};

	class Component
	{
	public:
		Component(ComponentType type) : m_type(type) {}
		virtual ~Component() = default;

		virtual void Tick(float dt) {};

		ComponentType GetType() const { return m_type; }

	protected:
		std::weak_ptr<Node> m_owner;
		ComponentType m_type;
	
		friend class Node;
		friend class Scene;
	};
}