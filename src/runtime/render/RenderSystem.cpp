#include "RenderSystem.h"

#include "runtime/rhi/RHIDevice.h"

#include <memory>

namespace vkR
{
	void RenderSystem::Initialize(const RenderSystemInitInfo& initInfo)
	{
		rhi::RHIInitInfo rhiInitInfo;
		rhiInitInfo.windowSystem = initInfo.windowSystem;

		m_device = std::make_shared<rhi::Device>();
		m_device->Initialize(rhiInitInfo);
	}

	void RenderSystem::Tick()
	{ 

	}

	void RenderSystem::Shutdown()
	{

	}
}