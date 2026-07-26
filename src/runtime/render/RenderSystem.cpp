#include "RenderSystem.h"
#include "runtime/log/Log.h"
#include "runtime/global/Engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/rhi/RHIDefinitions.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHICommandList.h"
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
		m_rhiGraphicsQueue = m_rhi->GetQueue({ .type = RHIQueueType::Graphics, .index = 0 });
		assert(m_rhiGraphicsQueue);
		// m_rhiComputeQueue = m_rhi->GetQueue({ .type = RHIQueueType::Compute, .index = 0 });
		// assert(m_rhiComputeQueue);
	
		m_rhiCmdPool = m_rhi->CreateCommandPool({ .queue = m_rhiGraphicsQueue });
		assert(m_rhiCmdPool);

		RHICommandList::Init(true);	// bypass = true
		m_rhiCmdList = RHICommandList::Get();	// command list is global, g_rhiCmdLIst;
		assert(m_rhiCmdList);

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