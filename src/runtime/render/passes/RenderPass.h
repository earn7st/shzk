#pragma once

#include <cstdint>
#include <memory>

namespace shzk
{
	enum class PassType : uint32_t
	{
		DepthPre				= 0,
		Forward					= 1,
		Sky						= 2,
		DirectionalShadowMap	= 3,
		PointLightShadowMap		= 4,

		Max,
	};

	enum class MeshPassType : uint32_t
	{
		DepthPre				= 0,
		Forward					= 1,
		DirectionalShadowMap	= 2,
		PointLightShadowMap		= 3,

		Max,
	};

	class RHICommandList;

	class RenderPass
	{
	public:
		RenderPass() = delete;
		RenderPass(PassType type) : m_type(type) {}
		~RenderPass() = default;

		virtual void Init() = 0;
		virtual void Prepare() = 0;
		virtual void Execute(std::shared_ptr<RHICommandList> cmd) = 0;

		PassType GetType() const { return m_type; }

	protected:
		PassType	m_type = PassType::Max;
		bool		m_enabled = true;
	};
}