#pragma once

#include <memory>
#include <vma/vk_mem_alloc.h>

#include "render/FrameSlot.h"
#include "render/pipeline/Pipeline.h"

struct SDLWindow;
struct VulkanContext;
struct SwapchainContext;
struct UploadContext;
class DescriptorAllocator;
class DescriptorSetLayoutCache;
class Scene;
struct RenderView;

#define FRAME_OVERLAP 2

class Renderer
{
private:
	std::unique_ptr<VulkanContext> vulkanContext;
	std::unique_ptr<SwapchainContext> swapchainContext;
	std::unique_ptr<UploadContext> uploadContext;

	std::unique_ptr<DescriptorAllocator> descAllocator;
	std::unique_ptr<DescriptorSetLayoutCache> descSetLayoutCache;

	Pipeline defaultPipeline;

	uint32_t currentFrame = 0;
	std::array<FrameSlot, FRAME_OVERLAP> frameSlots;

public:
	Renderer();
	Renderer(const SDLWindow& window);

	~Renderer();

	VulkanContext* GetVulkanContext() { return vulkanContext.get(); }
	SwapchainContext* GetSwapchainContext() { return swapchainContext.get(); }
	UploadContext* GetUploadContext() { return uploadContext.get(); }
	DescriptorAllocator* GetDescriptorAllocator() { return descAllocator.get(); }
	DescriptorSetLayoutCache* GetDescriptorSetLayoutCache() { return descSetLayoutCache.get();  }

	uint32_t GetCurrentFrame() { return currentFrame;  }

	void ResizeSwapchain(uint32_t width, uint32_t height);
	void DestroySwapchain();

	void DrawFrame(const RenderView& view);
};

namespace
{
	void InitSlotCommand(
		vk::Device device,
		uint32_t graphicsQueueFamily,
		FrameSlot& slot);

	void InitSlotSync(
		vk::Device device,
		FrameSlot& slot);

	void InitSlotDepth(
		VmaAllocator allocator,
		vk::Device device,
		vk::Extent2D extent,
		vk::Format format,
		FrameSlot& slot);

	void InitSlotUniform(
		VmaAllocator allocator,
		vk::Device device,
		DescriptorAllocator& descAllocator,
		vk::DescriptorSetLayout layout,
		FrameSlot& slot);

	void DestroySlotCommand(vk::Device device, FrameSlot& slot);
	void DestroySlotSync(vk::Device device, FrameSlot& slot);
	void DestroySlotDepth(VmaAllocator allocator, vk::Device device, FrameSlot& slot);
	void DestroySlotUniform(VmaAllocator allocator, FrameSlot& slot);
}
