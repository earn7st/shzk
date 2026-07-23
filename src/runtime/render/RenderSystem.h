#pragma once

#include <memory>

namespace shzk
{
	class RHI;

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Init();
		void Shutdown();

		void Tick();

	private:
		std::shared_ptr<RHI> m_rhi;
	};
}