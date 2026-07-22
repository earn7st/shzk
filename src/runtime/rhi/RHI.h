#pragma once

#include <memory>

namespace shzk
{
	class RHI
	{
	private:
		static std::shared_ptr<RHI> rhi;

	public:
		RHI() = default;
		~RHI() = default;

		virtual void Init();
		virtual void Shutdown();
	};
}