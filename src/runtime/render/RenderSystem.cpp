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
		// global rhi should be already initialized
		// in Engine::Init()
		m_rhi = RHI::Get();
		assert(m_rhi);
		
		m_rhiSurface = m_rhi->CreateSurface(Engine::GetWindowSystem()->GetWindow());
		assert(m_rhiSurface);

		RHIQueueInfo graphicsQueueInfo{};
		graphicsQueueInfo.type	= RHIQueueType::Graphics;
		graphicsQueueInfo.index = 0;
		m_rhiGraphicsQueue = m_rhi->GetQueue(graphicsQueueInfo);
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
		m_rhiSwapchain = m_rhi->CreateSwapchain(swapchainInfo);

		RHICommandPoolInfo cmdPoolInfo{};
		cmdPoolInfo.queue = m_rhiGraphicsQueue;
		m_rhiCmdPool = m_rhi->CreateCommandPool(cmdPoolInfo);
		assert(m_rhiCmdPool);

		RHICommandList::Init(true);				// bypass = true
		m_rhiCmdList = RHICommandList::Get();	// command list is global, g_rhiCmdLIst;
		assert(m_rhiCmdList);

		m_rhiCmdListImmediate = RHICommandListImmediate::Get();
		m_rhiCmdListImmediate->SetCommandContext(RHI::Get()->GetCommandContextImmediate().get());
		assert(m_rhiCmdListImmediate);

		InitPerFrameRHIResources();
	}

	void RenderSystem::Shutdown()
	{

	}

	void RenderSystem::Tick()
	{

	}

// --- private functions ---
	void RenderSystem::InitPerFrameRHIResources()
	{
		for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
		{
			m_perFrameResources[i].cmdContext		= m_rhiCmdPool->CreateCommandContext();
			m_perFrameResources[i].startSemaphore	= m_rhi->CreateSemaphore();
			m_perFrameResources[i].endSemaphore		= m_rhi->CreateSemaphore();
			m_perFrameResources[i].fence			= m_rhi->CreateFence();
		}
	}
}