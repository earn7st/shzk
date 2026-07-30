#pragma once

#include "runtime/rhi/RHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHITexture : public RHITexture
	{
	public:
		VulkanRHITexture() = delete;
		VulkanRHITexture(const RHITextureInfo& info, VulkanRHI& rhi, VkImage image = VK_NULL_HANDLE);
		~VulkanRHITexture() = default;

		inline const VkImage& GetHandle() { return m_handle; }

	protected:
		virtual void Destroy() override final;

	private:
		VkImage m_handle;

		VmaAllocation m_alloc = nullptr;
		VmaAllocationInfo m_allocInfo{};
	};
}