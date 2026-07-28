#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHIQueue : public RHIQueue
	{
	public:
		VulkanRHIQueue(const RHIQueueInfo& info, VkQueue queue, uint32_t queueFamilyIndex)
			: RHIQueue(info), m_handle(queue), m_queueFamilyIndex(queueFamilyIndex) {}

		// Getters
		inline VkQueue GetHandle() const { return m_handle; }
		inline uint32_t GetQueueFamilyIndex() const { return m_queueFamilyIndex; }

		virtual void WaitIdle() override final;

	private:
		VkQueue m_handle;
		uint32_t m_queueFamilyIndex;
	};
}