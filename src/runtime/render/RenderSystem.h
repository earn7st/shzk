#pragma once

#include <memory>

namespace shzk
{
	class RHI;
	class RHISurface;
	class RHISwapchain;
	class RHIQueue;

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Init();
		void Shutdown();

		void Tick();

	private:
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr<RHISurface> m_rhiSurface;

		std::shared_ptr<RHIQueue> m_rhiGraphicsQueue;
		//std::shared_ptr<RHISwapchain> m_rhiSwapchain;
		//std::shared_ptr<RHIQueue> m_rhiComputeQueue;
	};
}