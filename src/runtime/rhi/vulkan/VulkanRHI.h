#pragma once

#include "runtime/rhi/RHI.h"

namespace shzk
{
	class VulkanRHI : public RHI
	{
	public:
		VulkanRHI() = default;
		~VulkanRHI() = default;

		virtual void Init() final override;
	};
}