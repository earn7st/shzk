#pragma once

#include <cstdint>

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

	class RenderPass
	{
	public:
		RenderPass() = delete;
		RenderPass(PassType type) : m_type(type) {}
		~RenderPass() = default;

		virtual void Init() = 0;
		PassType GetType() const { return m_type; }

	protected:
		PassType	m_type = PassType::Max;
		bool		m_enabled = true;
	};
}