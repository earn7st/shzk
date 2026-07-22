#pragma once

namespace shzk
{
	class RHIBackend;

	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		void Init();
		void Shutdown();

		void Tick();

	private:
		std::shared_ptr<RHIBackend> m_rhiBackend;
	};
}