#pragma once

#include "runtime/rhi/RHIDefinitions.h"

#include <memory>
#include <array>

namespace shzk
{
	class RHI;
	class RHISurface;
	class RHISwapchain;
	class RHIQueue;
	class RHICommandList;
	class RHICommandPool;
	class RHICommandContext;
	class RHISemaphore;
	class RHIFence;

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Init();
		void Shutdown();

		void Tick();

	private:
		void InitPerFrameRHIResources();

	private:
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr<RHISurface> m_rhiSurface;
		std::shared_ptr<RHIQueue> m_rhiGraphicsQueue;
		//std::shared_ptr<RHIQueue> m_rhiComputeQueue;

		std::shared_ptr<RHICommandPool> m_rhiCmdPool;
		std::shared_ptr<RHICommandList> m_rhiCmdList;
		//std::shared_ptr<RHISwapchain> m_rhiSwapchain;

		struct PerFrameRHIResource
		{
			std::shared_ptr<RHICommandContext> cmdContext;
			std::shared_ptr<RHISemaphore> startSemaphore;
			std::shared_ptr<RHISemaphore> endSemaphore;
			std::shared_ptr<RHIFence> fence;
		};
		std::array<PerFrameRHIResource, FRAMES_IN_FLIGHT> m_perFrameResources;
	};
}