#pragma once

#include <memory>

namespace vkR
{
	class WindowSystem;

	namespace rhi
	{
		class Device;
	}

	struct RenderSystemInitInfo
	{
		std::shared_ptr<WindowSystem> windowSystem;
	};

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Initialize(const RenderSystemInitInfo& initInfo);
		void Tick();
		void Shutdown();

	private:
		std::shared_ptr<rhi::Device> m_device;

			
	};
}