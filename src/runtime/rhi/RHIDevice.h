#pragma once

#include "RHIStructs.h"

#include <vector>
#include <memory>

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <vma/vk_mem_alloc.h>


namespace vkR
{
	class WindowSystem;

	namespace rhi
	{

		struct RHIInitInfo
		{
			std::shared_ptr<WindowSystem> windowSystem;
		};

		class Device
		{
		public:
			Device() = default;

			void Initialize(const RHIInitInfo& initInfo);
		

		private:
			void CreateInstance();
			void CreateSurface();
			void CreateDeviceAndQueues();
			void CreateAllocator();
			
			void CreateSwapchain();
			void CreateImageViews();

			void CreateImmediateCommandPool();
			void CreateFrameCommandPools();
			void CreateFrameCommandBuffers();
			void CreateFrameSyncs();

			SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
			VkSurfaceFormatKHR ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities);

		private:
			VkDebugUtilsMessengerEXT m_debugMessenger = nullptr;
			const std::vector<char const*> m_validationLayers{ "VK_LAYER_KHRONOS_validation" };

			static uint32_t kFramesInFlight;

			SDL_Window* m_window;

			// --- Device ---
			VkInstance m_instance;
			VkSurfaceKHR m_surface;
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			uint32_t m_graphicsQueueFamily;
			VkQueue m_graphicsQueue;

			// --- Swapchain ---
			VkSwapchainKHR m_swapchain;
			VkFormat m_swapchainFormat;
			VkExtent2D m_swapchainExtent;
			std::vector<VkImage> m_swapchainImages;
			std::vector<VkImageView> m_swapchainImageViews;

			VkRect2D m_scissor;

			// --- Allocator ---
			VmaAllocator m_allocator;

			// --- Immediate Submit ---
			VkFence m_immediateFence;
			VkCommandPool m_immediateCommandPool;

			// --- Frames ---
			std::vector<VkCommandPool>		m_commandPools;
			std::vector<VkCommandBuffer>	m_commandBuffers;

			// --- Synchronization ---
			std::vector<VkFence>			m_fences;
			std::vector<VkSemaphore>		m_imageAcquiredSemaphores;
			std::vector<VkSemaphore>		m_renderFinishedSemaphore;

			QueueFamilyIndices m_queueFamilyIndices;

		};
	}
	
}