#pragma once

#include <memory>

class SDL_Window;

namespace shzk
{
	class RHIQueue;
	class RHISurface;
	class RHISwapchain;

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

		//virtual std::shared_ptr<RHIQueue> GetQueue() = 0;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) = 0;
		//virtual std::shared_ptr<RHISwapchain> CreateSwapchain() = 0;

	protected:
		RHI() = delete;
		RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}

		RHIInfo m_rhiInfo;
	};
}