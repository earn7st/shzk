#include "Renderer.h"

#include "backend/VulkanContext.h"
#include "platform/SDLWindow.h"
#include "scene/Scene.h"

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::Init(const SDLWindow& window)
{
	_vkContext = std::make_unique<VulkanContext>();
	_vkContext->Init(window);

}

void Renderer::Draw(const Scene& scene)
{
	const DrawContext drawContext = scene.BuildDrawContext();
	(void)drawContext;
}
