#include "RenderSystem.h"
#include "runtime/log/Log.h"
#include "runtime/global/Engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <cassert>

namespace shzk
{
	void RenderSystem::Init()
	{
		// global rhi should be already initialized
		// in Engine::Init()
		m_rhi = RHI::Get();
		if (!m_rhi)
		{
			assert(false);
			SHZK_LOG_ERROR("RHI not initialized!");
			return;
		}
		
		m_rhiSurface = m_rhi->CreateSurface(Engine::GetWindowSystem()->GetWindow());
		assert(m_rhiSurface);
		m_rhiGraphicsQueue = m_rhi->GetQueue({ .type = RHIQueueType::Graphics, .index = 0 });

	}

	void RenderSystem::Shutdown()
	{

	}

	void RenderSystem::Tick()
	{

	}
}