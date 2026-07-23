#include "RenderSystem.h"
#include "runtime/global/Engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/rhi/RHI.h"

#include <cassert>

namespace shzk
{
	void RenderSystem::Init()
	{
		// global rhi should be already initialized
		// in Engine::Init()
		m_rhi = RHI::Get();
		
		m_rhiSurface = m_rhi->CreateSurface(Engine::GetWindowSystem()->GetWindow());
		assert(m_rhiSurface);


	}

	void RenderSystem::Shutdown()
	{

	}

	void RenderSystem::Tick()
	{

	}
}