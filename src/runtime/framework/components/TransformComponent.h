#pragma once

#include "Component.h"
#include "runtime/core/Definitions.h"

namespace shzk
{
	class TransformComponent : public Component
	{

	private:
		Transform m_transform;
	};
}