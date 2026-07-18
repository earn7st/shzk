#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

namespace vkR
{
	namespace rhi
	{
		struct SwapchainSupportDetails;

		class CommandList;

		class Swapchain
		{
		public:
			void Initialize(
				SDL_Window* window,
				VkPhysicalDevice physicalDevice,
				VkDevice         device,
				VkSurfaceKHR     surface,
				const SwapchainSupportDetails& swapchainSupport);
			void Shutdown();

			void AcquireNextImage();
			void Present(CommandList* cmdListPresent);

			// --- Getters ---
			VkSemaphore GetCurrentImageAcquiredSemaphore() const { return m_imageAcquiredSemaphores[m_acquiredImageIndex]; }
			VkSemaphore GetCurrenRenderCompleteSemaphore() const { return m_renderCompleteSemaphores[m_acquiredImageIndex]; }
			VkImage GetCurrentImage() const { return m_images[m_acquiredImageIndex]; }
			VkImageView GetCurrentImageView() const { return m_imageViews[m_acquiredImageIndex]; }
			VkExtent2D GetExtent() const { return m_extent; }
			VkFormat GetFormat() const { return m_format; }
						
			bool IsImageAcquired() const { return m_imageAcquired; }

		private:
			VkSurfaceFormatKHR ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& formats);
			VkPresentModeKHR ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities);

		private:
			SDL_Window* m_window = nullptr;
			VkDevice m_device = VK_NULL_HANDLE;

			// --- Swapchain ---
			VkSurfaceKHR m_surface = VK_NULL_HANDLE;
			VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
			VkFormat m_format = VK_FORMAT_UNDEFINED;
			VkExtent2D m_extent = {};
			std::vector<VkImage> m_images;
			std::vector<VkImageView> m_imageViews;

			bool m_imageAcquired = false;
			uint32_t m_acquiredImageIndex = 0;

			// --- Syncs ---
			std::vector<VkSemaphore>		m_imageAcquiredSemaphores;
			std::vector<VkSemaphore>		m_renderCompleteSemaphores;
			uint32_t m_semaphoreIndex = 0;

			// --- Recreation ---
			bool m_isDirty = false;
		};
	}
}