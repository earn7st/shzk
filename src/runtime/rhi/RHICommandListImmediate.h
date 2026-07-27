#pragma once
#include "RHI.h"

#include <memory>

namespace shzk
{
	class RHICommandContextImmediate;

	class RHICommandListImmediate
	{
	private:
		static std::shared_ptr<RHICommandListImmediate> g_RhiCmdListImmediate;

	public:
		RHICommandListImmediate() = default;
		~RHICommandListImmediate() = default;

		static std::shared_ptr<RHICommandListImmediate> Get() { return g_RhiCmdListImmediate; }

		void SetCommandContext(RHICommandContextImmediate* cmdContext) { m_cmdContext = cmdContext; }	// 在初始化的时候被调用一次，设置 context 为 RHI 中维护的那一份 immediate context

	private:
		RHICommandContextImmediate* m_cmdContext = nullptr;
	};
}