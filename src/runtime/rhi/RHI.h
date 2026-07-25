#pragma once

#include "RHIDefinitions.h"

#include <memory>

class SDL_Window;

namespace shzk
{
	class RHIQueue;
	class RHISurface;
	class RHISwapchain;

	class RHI
	{
	private:
		static std::shared_ptr<RHI> rhi;

	public:
		static std::shared_ptr<RHI> Init(const RHIInfo& rhiInfo);
		static void Shutdown();
		static std::shared_ptr<RHI> Get() { return rhi; }

		virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) = 0;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) = 0;
		//virtual std::shared_ptr<RHISwapchain> CreateSwapchain() = 0;

	protected:
		RHI() = delete;
		RHI(const RHIInfo& rhiInfo) : m_rhiInfo(rhiInfo) {}

		RHIInfo m_rhiInfo;
	};

	class RHIQueue
	{
	public:
		RHIQueue(const RHIQueueInfo& info) : m_info(info) {}
		~RHIQueue() = default;

		inline RHIQueueType GetType() const { return m_info.type; }

	protected:
		RHIQueueInfo m_info;
	};

	class RHISurface
	{
	public:
		RHISurface() = default;
		inline Extent2D GetExetent() const { return m_extent; }

	protected:
		Extent2D m_extent;
	};

	class RHISwapchain
	{
	public:
		RHISwapchain() = delete;
		RHISwapchain(const RHISwapchainInfo& info) : m_info(info) {}

	private:
		RHISwapchainInfo m_info;
	};

	class RHISemaphore
	{
	public:
	};

	class RHIFence
	{

	};

	class RHICommandPool
	{
	public:
		RHICommandPool(const RHICommandPoolInfo& info)
			: m_info(info) {}

	private:
		RHICommandPoolInfo m_info;
	};

	class RHICommandContext
	{
	public:
		RHICommandContext() = delete;

		virtual void Begin() = 0;
		virtual void End() = 0;
		//virtual void Submit(std::shared_ptr<RHISemaphore> wait,
		//	std::shared_ptr<RHISemaphore> signal,
		//	std::shared_ptr<RHIFence> fence) = 0;
		//virtual void DrawIndexed() = 0;
		//virtual void SetGraphicsPipeline() = 0;
		//virtual void BindVertexBuffer() = 0;
	};
}
