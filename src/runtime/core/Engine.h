#pragma once

#include <memory>

#include "WindowSystem.h"

namespace vkR
{
	class RenderSystem;

	struct EngineSpec
	{
		WindowSpec windowSpec;
	};

	class Engine
	{
	public:
		void Initialize(const EngineSpec& engineSpec);
		void Tick();
		void Shutdown();

		bool ShouldClose() const { return m_windowSystem->ShouldClose(); }

	private:
		std::shared_ptr<WindowSystem> m_windowSystem;
		std::shared_ptr<RenderSystem> m_renderSystem;
	};
}