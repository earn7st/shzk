#pragma once

#include "runtime/rhi/RHIDefinitions.h"

#include <memory>
#include <array>

namespace shzk
{
	// static const RHIFormat HDR_COLOR_FORMAT = FORMAT_R16G16B16A16_SFLOAT;
	static const RHIFormat COLOR_FORMAT = FORMAT_B8G8R8A8_UNORM;
	static const RHIFormat DEPTH_FORMAT = FORMAT_D32_SFLOAT;

	class RHI;
	class RHISurface;
	class RHISwapchain;
	class RHIQueue;
	class RHICommandList;
	class RHICommandListImmediate;
	class RHICommandPool;
	class RHICommandContext;
	class RHISemaphore;
	class RHIFence;
	class RHIGraphicsPipeline;

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
		std::shared_ptr<RHISurface> m_rhiSurface;
		std::shared_ptr<RHISwapchain> m_rhiSwapchain;
		std::shared_ptr<RHIQueue> m_rhiGraphicsQueue;
		//std::shared_ptr<RHIQueue> m_rhiComputeQueue;

		std::shared_ptr<RHICommandPool> m_rhiCmdPool;
		struct PerFrameRHIResource
		{
			std::shared_ptr<RHICommandContext> cmdContext;
			std::shared_ptr<RHISemaphore> startSemaphore;
			std::shared_ptr<RHISemaphore> endSemaphore;
			std::shared_ptr<RHIFence> fence;
		};
		std::array<PerFrameRHIResource, FRAMES_IN_FLIGHT> m_perFrameResources;
		uint32_t m_currentFrameIndex = 0;
	};
}