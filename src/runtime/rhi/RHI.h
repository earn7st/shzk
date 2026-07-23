#pragma once

#include <memory>

namespace shzk
{
	enum class RHIBackendType
	{
		Vulkan,
		Max
	};

	struct RHIInfo
	{
		RHIBackendType type = RHIBackendType::Vulkan;
		bool debug = true;
	};

	class RHI
	{
	private:
		static std::shared_ptr<RHI> rhi;

	public:
		static std::shared_ptr<RHI> Init(const RHIInfo& rhiInfo);
		static void Shutdown();
		static std::shared_ptr<RHI> Get() { return rhi; }

	protected:
		RHI() = delete;
		RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}

		RHIInfo m_rhiInfo;
	};
}