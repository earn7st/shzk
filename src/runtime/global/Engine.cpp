#include "engine.h"
#include "runtime/WindowSystem.h"
#include "runtime/render/RenderSystem.h"
#include "runtime/rhi/RHI.h"

namespace shzk
{
	std::shared_ptr<Engine> Engine::engine = std::make_shared<Engine>();

	void Engine::Init(const EngineInitInfo& engineInitInfo)
	{
		WindowSystemInitInfo windowInitInfo{};
		windowInitInfo.title = engineInitInfo.name.c_str();
		windowInitInfo.width = engineInitInfo.width;
		windowInitInfo.height = engineInitInfo.height;
		engine->m_windowSystem = std::make_shared<WindowSystem>();
		engine->m_windowSystem->Init(windowInitInfo);
	
		engine->m_rhi = RHI::Init({ .type = RHIBackendType::Vulkan, .debug = true});

		engine->m_renderSystem = std::make_shared<RenderSystem>();
		engine->m_renderSystem->Init();

	}

	void Engine::Tick()
	{
		engine->m_windowSystem->Tick();
		engine->m_renderSystem->Tick();
	}

	void Engine::Shutdown()
	{
		engine->m_renderSystem->Shutdown();
		engine->m_windowSystem->Shutdown();
	}

	bool Engine::ShouldClose()
	{
		return engine->m_windowSystem->ShouldClose();
	}

}
