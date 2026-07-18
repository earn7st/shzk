#pragma once

#include <memory>

namespace vkR
{
	class WindowSystem;

	namespace rhi
	{
		class Device;
		class CommandList;
		class Swapchain;
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

		void ProduceFrame();
		void SubmitAndPresent();

	private:
		std::shared_ptr<rhi::Device> m_device;
		std::shared_ptr<rhi::Swapchain> m_swapchain;
		rhi::CommandList* m_currentCmdListGraphics = nullptr;
		rhi::CommandList* m_currentCmdListCompute = nullptr;
	};
}