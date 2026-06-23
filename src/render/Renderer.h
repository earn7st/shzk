#pragma once

#include <memory>

struct SDLWindow;
struct VulkanContext;
struct SwapchainContext;
struct UploadContext;
class Scene;


class Renderer
{
private:
	std::unique_ptr<VulkanContext> vulkanContext;
	std::unique_ptr<SwapchainContext> swapchainContext;
	std::unique_ptr<UploadContext> uploadContext;

public:
	Renderer();
	Renderer(const SDLWindow& window);

	~Renderer();

	VulkanContext* GetVulkanContext() { return vulkanContext.get(); }
	SwapchainContext* GetSwapchainContext() { return swapchainContext.get(); }
	UploadContext* GetUpladContext() { return uploadContext.get(); }
};
