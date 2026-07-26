#include "VulkanRHISurface.h"
#include "VulkanRHI.h"
#include "VulkanUtil.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace shzk
{
	VulkanRHISurface::VulkanRHISurface(SDL_Window* window, VulkanRHI& rhi)
		: RHISurface()
	{
		assert(window);
		if (!SDL_Vulkan_CreateSurface(window, rhi.GetInstance(), nullptr, &m_handle))
		{
			SHZK_LOG_ERROR("Failed to create surface.");
		}

		// RHISurface
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		m_extent.width = static_cast<uint32_t>(w);
		m_extent.height = static_cast<uint32_t>(h);
	}
}