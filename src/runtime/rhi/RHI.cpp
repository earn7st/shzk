#include "RHI.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/vulkan/VulkanRHI.h"

namespace shzk
{
	std::shared_ptr<RHI> RHI::rhi = nullptr;

	std::shared_ptr<RHI> RHI::Init(const RHIInfo& rhiInfo)
	{
		switch (rhiInfo.type)
		{
		case RHIBackendType::Vulkan:
			SHZK_LOG_INFO("RHIBackendType: Vulkan");
			rhi = std::make_shared<VulkanRHI>(rhiInfo);
			break;
		case RHIBackendType::Max:
			SHZK_LOG_WARN("RHIBackendType: Max(not defined), Vulkan by default.");
			break;
		default:
			SHZK_LOG_ERROR("RHIBackendType Invalid.");
			return nullptr;
		}
		return rhi;
	}
}