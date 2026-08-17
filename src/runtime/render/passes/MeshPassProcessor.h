#pragma once

#include "runtime/rhi/RHIDefinitions.h"
#include "runtime/render/MeshBatch.h"

#include <vector>

namespace shzk
{
	typedef struct GraphicsMinimalPipelineState
	{
		BoundShaderStateInput boundShaderStateInput{};
		RHIBlendState blendState{};
		RHIRasterizerState rasterizerState{};
		RHIDepthStencilState depthStencilState{};
		PrimitiveType primitiveType = PrimitiveType::TriangleList;

		friend bool operator==(const GraphicsMinimalPipelineState& a, const GraphicsMinimalPipelineState& b)
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

		void Init();
		void Process(const std::vector<MeshBatch>& batches);

		virtual void BuildMeshDrawCommand();

	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) = 0; 

		friend class MeshPass;

	private:
		std::vector<MeshBatch>	m_batches;
		
	};
}