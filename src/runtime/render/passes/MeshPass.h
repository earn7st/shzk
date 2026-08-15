#pragma once
#include "RenderPass.h"

#include <memory>

namespace shzk
{
	class MeshPassProcessor;

	class MeshPass : public RenderPass
	{
	public:
		MeshPass() = delete;
		MeshPass(PassType type) : RenderPass(type) {}
		~MeshPass() = default;

		virtual void Init() override;

		std::shared_ptr<MeshPassProcessor> GetMeshPassProcessor() const { return m_meshPassProcessor; }

	protected:
		std::shared_ptr<MeshPassProcessor> m_meshPassProcessor;

		
	};
}