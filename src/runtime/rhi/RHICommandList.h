#pragma once

#include <memory>

namespace shzk
{
	class RHICommandContext;
	class RHICommandList
	{
	private:
		static std::shared_ptr<RHICommandList> g_RhiCmdList;

	public:
		RHICommandList() = default;
		~RHICommandList() = default;

		static void Init();

		void Begin() = 0;
		void End() = 0;
		// void Submit(std::shared_ptr<RHISemaphore> wait,
		//	std::shared_ptr<RHISemaphore> signal,
		//	std::shared_ptr<RHIFence> fence) = 0;
		// void DrawIndexed() = 0;
		// void SetGraphicsPipeline() = 0;
		// void BindVertexBuffer() = 0;

	private:
		RHICommandContext* m_cmdContext;
		bool deferred = false;
	};
}