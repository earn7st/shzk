#include "RHI.h"
#include "runtime/log/Log.h"
#include "runtime/rhi/vulkan/VulkanRHI.h"
#include "RHIResource.h"

namespace shzk
{
	std::shared_ptr<RHI> RHI::g_rhi = nullptr;

	std::shared_ptr<RHI> RHI::Init(const RHIInfo& rhiInfo)
	{
		switch (rhiInfo.type)
		{
		case RHIBackendType::Vulkan:
			SHZK_LOG_INFO("RHI backend type: Vulkan");
			g_rhi = std::make_shared<VulkanRHI>(rhiInfo);
			break;
		case RHIBackendType::Max:
			SHZK_LOG_WARN("RHI backend type: Max(not defined), Vulkan by default.");
			break;
		default:
			SHZK_LOG_ERROR("RHI backend type Invalid.");
			break;
		}
		return g_rhi;
	}

	void RHI::Tick()
	{
		DeferredResourceDeletes();
	}

	void RHI::RegisterResource(std::shared_ptr<RHIResource> res)
	{
		m_resourceMap[(size_t)res->GetType()].push_back(res);
	};


	void RHI::DeferredResourceDeletes()
	{
		for (auto& resources : m_resourceMap)
		{
			for (auto& res : resources)
			{
				if (!res) continue;
				if (res.use_count() == 1)	res->m_framesLastUsed++;
				else						res->m_framesLastUsed = 0;

				if (res->m_framesLastUsed > RESOURCE_DEFERRED_DELETE_FRAMES)
				{
					res->Destroy();
					res = nullptr;	
				}
			}
			resources.erase(
				std::remove(resources.begin(), resources.end(), nullptr),
				resources.end());
		}
	}

	void RHI::DestroyAllResources()
	{
		for (auto& resources : m_resourceMap)
		{
			for (auto& res : resources)
			{
				if (!res) continue;
				res->Destroy();
			}
		}
	}

}