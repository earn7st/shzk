#pragma once
#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHISemaphore : public RHISemaphore
	{
	public:
		VulkanRHISemaphore() = delete;
		VulkanRHISemaphore(VulkanRHI& rhi);
		~VulkanRHISemaphore() = default;
	
		virtual void Destroy() override final;

		inline const VkSemaphore& GetHandle() { return m_handle; }

	private:
		VkSemaphore m_handle;
	};
}