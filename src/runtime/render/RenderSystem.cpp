#include "RenderSystem.h"
#include "runtime/log/Log.h"
#include "runtime/global/Engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/rhi/RHIDefinitions.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHICommandList.h"
#include "runtime/rhi/RHICommandListImmediate.h"
#include <cassert>

namespace shzk
{
// --- public functions ---
	void RenderSystem::Init()
	{
		
		m_rhiSurface = RHI::Get()->CreateSurface(Engine::GetWindowSystem()->GetWindow());
		assert(m_rhiSurface);

		RHIQueueInfo graphicsQueueInfo{};
		graphicsQueueInfo.type	= RHIQueueType::Graphics;
		graphicsQueueInfo.index = 0;
		m_rhiGraphicsQueue = RHI::Get()->GetQueue(graphicsQueueInfo);
		assert(m_rhiGraphicsQueue);
		// RHIQueueInfo computeQueueInfo{};
		// computeQueueInfo.type = RHIQueueType::Compute;
		// computeQueueInfo.index = 0;
		// m_rhiComputeQueue = m_rhi->GetQueue(computeQueueinfo);
		// assert(m_rhiComputeQueue);

		RHISwapchainInfo swapchainInfo{};
		swapchainInfo.surface		= m_rhiSurface;
		swapchainInfo.presentQueue	= m_rhiGraphicsQueue;
		swapchainInfo.imageCount	= FRAMES_IN_FLIGHT;
		swapchainInfo.extent		= m_rhiSurface->GetExetent();
		swapchainInfo.format		= COLOR_FORMAT;
		m_rhiSwapchain = RHI::Get()->CreateSwapchain(swapchainInfo);

		RHICommandPoolInfo cmdPoolInfo{};
		cmdPoolInfo.queue = m_rhiGraphicsQueue;
		m_rhiCmdPool = RHI::Get()->CreateCommandPool(cmdPoolInfo);
		assert(m_rhiCmdPool);

		RHICommandList::Init(true);				// bypass = true
		RHICommandListImmediate::Get()->SetCommandContext(RHI::Get()->GetCommandContextImmediate().get());

		InitPerFrameRHIResources();
	}

	void RenderSystem::Shutdown()
	{
		for (auto& resource : m_perFrameResources)
		{
			resource.fence->Destroy();
			resource.startSemaphore->Destroy();
			resource.endSemaphore->Destroy();
			resource.cmdContext->Destroy();
		}

		m_rhiCmdPool->Destroy();
		m_rhiSwapchain->Destroy();
		m_rhiSurface->Destroy();
	}

	void RenderSystem::Tick()
	{
		PerFrameRHIResource& resource = m_perFrameResources[m_currentFrameIndex];
		//resource.fence->Wait();

		//RHICommandList::Get()->BeginCommand();


		//RHICommandList::Get()->EndCommand();
	}

// --- private functions ---
	void RenderSystem::InitPerFrameRHIResources()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
		{
			m_perFrameResources[i].cmdContext		= m_rhiCmdPool->CreateCommandContext();
			m_perFrameResources[i].startSemaphore	= RHI::Get()->CreateSemaphore();
			m_perFrameResources[i].endSemaphore		= RHI::Get()->CreateSemaphore();
			m_perFrameResources[i].fence			= RHI::Get()->CreateFence();
		}
	}
}