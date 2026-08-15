#pragma once

#include "runtime/rhi/RHIDefinitions.h"

#include <vector>

namespace shzk
{
	struct MeshBatch;

	typedef struct GraphicsMinimalPipelineState
	{
		// BoundShaderState
		RHIBlendState blendState{};
		RHIRasterizerState rasterizerState{};
		RHIDepthStencilState depthStencilState{};
		PrimitiveType primitiveType = PrimitiveType::TriangleList;

		friend bool operator == (const GraphicsMinimalPipelineState& a, const GraphicsMinimalPipelineState& b)
		{
			return	a.blendState == b.blendState &&
				a.rasterizerState == b.rasterizerState &&
				a.depthStencilState == b.depthStencilState &&
				a.primitiveType == b.primitiveType;
		}

	} GraphicsMinimalPipelineState;

	class MeshPassProcessor
	{
	public:
		MeshPassProcessor() = default;
		~MeshPassProcessor() = default;

		virtual void BuildMeshDrawCommand();

	protected:
		virtual void Init();
		virtual void AddMeshBatch() = 0;

		friend class MeshPass;

	private:
		std::vector<MeshBatch>	m_batches;
		
	};
}