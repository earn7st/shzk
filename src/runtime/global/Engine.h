#pragma once

#include <memory>
#include <string>
#include <chrono>

namespace shzk
{
	class WindowSystem;
	class InputSystem;
	class RenderSystem;
	class RHI;
	class AssetManager;
	class RenderResourceManager;
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
		static std::shared_ptr<InputSystem>& GetInputSystem() { return g_engine->m_inputSystem; }
		static std::shared_ptr<RenderSystem>& GetRenderSystem() { return g_engine->m_renderSystem; }
		static std::shared_ptr<RHI>& GetRHI() { return g_engine->m_rhi; }

		void SetActiveScene(const std::shared_ptr<Scene>& scene) { m_activeScene = scene; }
		const std::shared_ptr<Scene>& GetActiveScene() { return m_activeScene; }

		float GetDeltaTime() const { return m_deltaTime; }

	private:
		std::shared_ptr<WindowSystem>	m_windowSystem;
		std::shared_ptr<InputSystem>	m_inputSystem;
		std::shared_ptr<RenderSystem>	m_renderSystem;
		std::shared_ptr<RHI>			m_rhi;				// ref to global rhi
		std::shared_ptr<AssetManager>	m_assetManager;		// ref to global asset manager
		std::shared_ptr<RenderResourceManager> m_renderResourceManager;

		std::shared_ptr<Scene> m_activeScene;	// TODO: World(SceneManager) -> Scenes, activeScene

		std::chrono::steady_clock::time_point m_lastTickTime{};
		float m_deltaTime = 0.f;
	};
}