#pragma once

#include "Component.h"

namespace vkR
{
	class MeshComponent : public Component
	{
	public:
		MeshComponent() = default;
		~MeshComponent() = default;

		void Initialize();
	};
}