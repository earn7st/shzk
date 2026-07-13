#include "RHIUtil.h"

#include <stdexcept>
#include <fmt/core.h>

namespace vkR::rhi
{
	VkImageView RHIUtil::CreateImageView(
		VkDevice           device,
		VkImage& image,
		VkFormat           format,
		VkImageAspectFlags image_aspect_flags,
		VkImageViewType    view_type,
		uint32_t           layout_count,
		uint32_t           miplevels)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = view_type;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = image_aspect_flags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = miplevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layout_count;
		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			fmt::println("[Vulkan Error] Failed to create image view");
			throw std::runtime_error("[Error] RHIUtils::CreatImageView() : Failed to create image view");
		}
		return imageView;
	}
}