#pragma once

#include <memory>

struct SDLWindow;
struct VulkanContext;
struct SwapchainContext;
class Scene;


class Renderer
{
private:
	std::unique_ptr<VulkanContext> vulkanContext;
	std::unique_ptr<SwapchainContext> swapchainContext;

public:
	Renderer();
	~Renderer();

	void Initialize(const SDLWindow& window);
};
