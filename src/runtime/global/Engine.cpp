#include "Engine.h"
#include "runtime/log/Log.h"
#include "runtime/WindowSystem.h"
#include "runtime/asset/AssetManager.h"
#include "runtime/framework/Scene.h"
#include "runtime/render/RenderSystem.h"
#include "runtime/render/resources/RenderResourceManager.h"
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

		g_engine->m_renderResourceManager = RenderResourceManager::Get();
		g_engine->m_renderResourceManager->Init();

		g_engine->m_renderSystem = std::make_shared<RenderSystem>();
		g_engine->m_renderSystem->Init();

		g_engine->m_assetManager = AssetManager::Get();
		g_engine->m_assetManager->Init();

	}

	void Engine::Tick()
	{
		g_engine->m_windowSystem->Tick();
		g_engine->m_renderSystem->Tick();

		g_engine->m_rhi->Tick();	// RHIResources deferred deletion
	}

	void Engine::Shutdown()
	{
		g_engine->m_activeScene->Clear();	// TODO: World Management

		g_engine->m_assetManager->Shutdown();
		g_engine->m_renderSystem->Shutdown();
		g_engine->m_rhi->Destroy();
		g_engine->m_rhi.reset();
		g_engine->m_windowSystem->Shutdown();
	}

	bool Engine::ShouldClose()
	{
		return g_engine->m_windowSystem->ShouldClose();
	}

}
