#pragma once

namespace vkR
{
	enum ComponentType
	{
		ComponentType_Transform,
		ComponentType_Mesh,
		ComponentType_Camera,
		ComponentType_Light,
	};

	class Component
	{
	public:
		Component() = default;
		~Component() = default;

		virtual void Tick(float deltaTime) = 0;
	
	protected:
		ComponentType m_type;
	};
}