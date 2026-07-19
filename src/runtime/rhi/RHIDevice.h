#pragma once

#include "RHIDefs.h"

#include <vector>
#include <memory>
#include <functional>

#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>
#include <vma/vk_mem_alloc.h>


namespace vkR
{
	class WindowSystem;

	namespace rhi
	{

		class CommandList;
		class Queue;
		class Swapchain;
		class Buffer;

		struct RHIInitInfo
		{
			std::shared_ptr<WindowSystem> windowSystem;
		};

		class Device
		{
		public:
			Device() = default;
			~Device() = default;

			void Initialize(const RHIInitInfo& initInfo);
			void Shutdown();

			void ImmediateSubmit(std::function<void(VkCommandBuffer)>&& func);

			// --- Buffer ---
			bool MemoryCreateBuffer(
				VkBuffer* outBuffer,
				VmaAllocation* outAllocation,
				void** outMappedData,
				VkDeviceSize                 size,
				VkBufferUsageFlags           usage,
				VkMemoryPropertyFlags        memoryFlags,
				const char* name);
			void MemoryDestroyBuffer(VkBuffer buffer, VmaAllocation allocation);
			void UploadDataToBuffer(VkBuffer dstBuffer, const void* data, VkDeviceSize size);

			// --- Image --- 
			bool MemoryCreateImage(
				VkImage*			outImage,
				VmaAllocation*		outAllocation,
				VkExtent3D			extent,
				VkFormat			format,
				VkImageUsageFlags	usage,
				uint32_t			mipLevels,
				VkSampleCountFlagBits samples,
				const char*			name
			);
			void MemoryDestroyImage(VkImage image, VmaAllocation allocation);
			void UploadDataToImage(
				VkImage             dstImage,
				VkExtent3D          extent,
				VkImageUsageFlags   usage,
				const void*			data,
				VkDeviceSize        size);

			// --- Getters ---
			VkDevice GetDevice() const { return m_device; }
			std::shared_ptr<Swapchain> GetSwapchain() const { return m_swapchain; }
			VmaAllocator GetAllocator() const { return m_allocator; }
			Queue* GetGraphicsQueue() const { return m_graphicsQueue.get(); }

		private:
			void CreateInstance();
			void CreatePhysicalAndLogicalDevice();
			void CreateAllocator();
			void CreateQueues();
			void CreateSurface();
			void CreateSwapchain();
			void CreateExtFunctions();

			void CreateImmediateCommandPoolGraphics();
			void CreateImmediateFence();

			SwapchainSupportDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
			VkSurfaceFormatKHR ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities);

		private:
			

			static uint32_t kFramesInFlight;

			SDL_Window* m_window = nullptr;

			// --- Device ---
			VkInstance m_instance = VK_NULL_HANDLE;
			VkSurfaceKHR m_surface = VK_NULL_HANDLE;
			VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
			VkDevice m_device = VK_NULL_HANDLE;

			VkRect2D m_scissor = {};

			// --- Allocator ---
			VmaAllocator m_allocator = VK_NULL_HANDLE;

			// --- Queues ---
			std::shared_ptr<Queue> m_graphicsQueue;
			std::shared_ptr<Queue> m_computeQueue;
			std::shared_ptr<Queue> m_immediateQueue;

			// --- Swapchain ---
			std::shared_ptr<Swapchain> m_swapchain;

			// --- Immediate Submit ---
			VkFence m_immediateFence = VK_NULL_HANDLE;
			VkCommandPool m_immediateCommandPool = VK_NULL_HANDLE;			

			// --- Debug Messenger --
			VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

			// --- Extension Functions ---
			PFN_vkSetDebugUtilsObjectNameEXT m_pfnSetDebugUtilsObjectNameEXT = nullptr;
		};
	}
	
}