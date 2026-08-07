#pragma once

#include <iostream>
#include <memory>
#include <string>

namespace shzk
{
	class WindowSystem;
	class RenderSystem;
	class RHI;
	class Scene;

	struct EngineInitInfo
	{
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	class Engine
	{
	private:
		static std::shared_ptr<Engine> g_engine;

	public:
		Engine() = default;
		~Engine() = default;

		static void Init(const EngineInitInfo& engineInitInfo);
		static void Tick();
		static void Shutdown();
		static bool ShouldClose();
		static std::shared_ptr<Engine>& Get() { return g_engine; }
		static std::shared_ptr<WindowSystem>& GetWindowSystem() { return g_engine->m_windowSystem; }
		static std::shared_ptr<RenderSystem>& GetRenderSystem() { return g_engine->m_renderSystem; }
		static std::shared_ptr<RHI>& GetRHI() { return g_engine->m_rhi; }

		void SetActiveScene(std::shared_ptr<Scene> scene) { m_activeScene = scene; }

	private:
		std::shared_ptr<WindowSystem> m_windowSystem;
		std::shared_ptr<RenderSystem> m_renderSystem;
		std::shared_ptr<RHI> m_rhi;		// a copy of the global rhi shared_ptr

		std::shared_ptr<Scene> m_activeScene;	// TODO: World(SceneManager) -> Scenes, activeScene
	};
}