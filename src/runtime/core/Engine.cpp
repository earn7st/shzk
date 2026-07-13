#include "Engine.h"

#include "runtime/core/WindowSystem.h"

#include "runtime/render/RenderSystem.h"

namespace vkR
{
	void Engine::Initialize(const EngineSpec& engineSpec)
	{
		m_windowSystem = std::make_shared<WindowSystem>();
		m_windowSystem->Initialize(engineSpec.windowSpec);

		RenderSystemInitInfo renderSystemInitInfo;
		renderSystemInitInfo.windowSystem = m_windowSystem;
		m_renderSystem = std::make_shared<RenderSystem>();
		m_renderSystem->Initialize(renderSystemInitInfo);
	
	}

	void Engine::Tick()
	{
		m_windowSystem->Tick();
		//m_renderSystem->Tick();
	}

	void Engine::Shutdown()
	{
		m_renderSystem->Shutdown();
		m_windowSystem->Shutdown();
	}
}