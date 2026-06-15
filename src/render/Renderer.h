#pragma once

#include "VKCommon.h"

struct VulkanContext;
class SDLWindow;
class Scene;

class Renderer
{
private:
	std::unique_ptr<VulkanContext> _vkContext;

public:
	Renderer();
	~Renderer();

	void Init(const SDLWindow& window);
	void Draw(const Scene& scene);
};
