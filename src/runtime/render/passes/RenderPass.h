#pragma once

#include <cstdint>
#include <memory>

namespace shzk
{
	enum class PassType : uint32_t
	{
		Forward					= 0,
		DirectionalShadowMap	= 1,
		PointLightShadowMap		= 2,

		Max,
	};

	enum class MeshPassType : uint32_t
	{
		Forward					= 0,
		DirectionalShadowMap	= 1,
		PointLightShadowMap		= 2,

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