#pragma once

#include <memory>
#include <string>

namespace shzk
{
	class WindowSystem;
	class RenderSystem;
	class RHI;

	struct EngineInitInfo
	{
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	class Engine
	{
	private:
		static std::shared_ptr<Engine> engine;

	public:
		Engine() = default;
		~Engine() = default;

		static void Init(const EngineInitInfo& engineInitInfo);
		static void Tick();
		static void Shutdown();
		static bool ShouldClose();
		static std::shared_ptr<Engine>& Get() { return engine; }
		static std::shared_ptr<WindowSystem>& GetWindowSystem() { return engine->m_windowSystem; }
		static std::shared_ptr<RenderSystem>& GetRenderSystem() { return engine->m_renderSystem; }

	private:
		std::shared_ptr<WindowSystem> m_windowSystem;
		std::shared_ptr<RenderSystem> m_renderSystem;
		std::shared_ptr<RHI> m_rhi;
	};
}