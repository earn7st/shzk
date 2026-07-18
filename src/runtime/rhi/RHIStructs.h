#pragma once

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

namespace vkR::rhi
{
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
}