#pragma once

#include "runtime/rhi/RHI.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHISwapchain : public RHISwapchain
	{
	public:
		VulkanRHISwapchain(const RHISwapchainInfo& info, VulkanRHI& rhi);

		virtual uint32_t GetCurrentFrameIndex() override final { return currentIndex; }
		// virtual RHITextureRef GetTexture(uint32_t index) override final { return textures[index]; }
		// virtual RHITextureRef GetNewFrame(RHIFenceRef fence, RHISemaphoreRef signalSemaphore) override final;
		virtual void Present(std::shared_ptr<RHISemaphore> waitSemaphore) override final;

		const VkSwapchainKHR& GetHandle() { return m_handle; }

		virtual void Destroy() override final;

	private:
		VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(VkFormat targetFormat);
		VkPresentModeKHR ChooseSwapchainPresentMode();
		VkExtent2D ChooseSwapchainExtent();

	private:
		VkSwapchainKHR m_handle;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkPresentModeKHR m_presentMode;

		std::vector<VkImage> m_images;
		VkFormat m_imageFormat;
		VkExtent2D m_imageExtent;

		// Capabilities
		VkSurfaceCapabilitiesKHR m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_availableFormats;
		std::vector<VkPresentModeKHR> m_availablePresentModes;

		//std::vector<std::shared_ptr<RHITexture>> textures;
		uint32_t currentIndex;
	};
}