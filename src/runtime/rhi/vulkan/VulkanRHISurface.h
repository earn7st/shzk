#pragma once
#include "runtime/rhi/RHI.h"
#include <vulkan/vulkan.h>

class SDL_Window;

namespace shzk
{	
	class VulkanRHI;

	class VulkanRHISurface : public RHISurface
	{
	public:
		VulkanRHISurface() = delete;
		VulkanRHISurface(SDL_Window* window, VulkanRHI& rhi);

		virtual void Destroy() override final;

		inline const VkSurfaceKHR& GetHandle() { return m_handle; }

	private:
		VkSurfaceKHR m_handle;
	};
}