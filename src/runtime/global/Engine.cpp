#include "Engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/render/RenderSystem.h"
#include "runtime/rhi/RHI.h"

namespace shzk
{
	std::shared_ptr<Engine> Engine::g_engine = std::make_shared<Engine>();

	void Engine::Init(const EngineInitInfo& engineInitInfo)
	{
		WindowSystemInitInfo windowInitInfo{};
		windowInitInfo.title = engineInitInfo.name.c_str();
		windowInitInfo.width = engineInitInfo.width;
		windowInitInfo.height = engineInitInfo.height;
		g_engine->m_windowSystem = std::make_shared<WindowSystem>();
		g_engine->m_windowSystem->Init(windowInitInfo);
	
		g_engine->m_rhi = RHI::Init({ .type = RHIBackendType::Vulkan, .debug = true});

		g_engine->m_renderSystem = std::make_shared<RenderSystem>();
		g_engine->m_renderSystem->Init();

	}

	void Engine::Tick()
	{
		g_engine->m_windowSystem->Tick();
		g_engine->m_renderSystem->Tick();
	}

	void Engine::Shutdown()
	{
		g_engine->m_renderSystem->Shutdown();
		g_engine->m_windowSystem->Shutdown();
	}

	bool Engine::ShouldClose()
	{
		return g_engine->m_windowSystem->ShouldClose();
	}

}
