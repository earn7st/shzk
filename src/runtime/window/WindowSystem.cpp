#include "WindowSystem.h"
#include "runtime/log/Log.h"

#include <SDL3/SDL.h>

namespace shzk
{
	void WindowSystem::Init(const WindowSystemInitInfo& windowSystemInitInfo)
	{
		SDL_Init(SDL_INIT_VIDEO);

		m_window = SDL_CreateWindow(
			windowSystemInitInfo.title,
			windowSystemInitInfo.width,
			windowSystemInitInfo.height,
			SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

		// 存储实际创建出的窗口尺寸：macOS 等平台可能因屏幕限制把窗口裁小（如请求 1920x1080 → 实际 1470x891）
		int actualWidth = 0, actualHeight = 0;
		SDL_GetWindowSize(m_window, &actualWidth, &actualHeight);
		m_width = static_cast<uint32_t>(actualWidth);
		m_height = static_cast<uint32_t>(actualHeight);
	}

	void WindowSystem::Shutdown()
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	void WindowSystem::Tick()
	{
		
	}

}