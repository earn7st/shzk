#pragma once

#include "runtime/render/MeshBatch.h"
#include "runtime/render/MeshDrawCommand.h"
#include "runtime/render/resources/VertexFactory.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vector>

namespace shzk
{
	typedef struct MeshPassProcessorRenderState
	{
		RHIBlendState	blendState{};
		RHIDepthStencilState depthStencilState{};
		uint32_t             stencilRef = 0;
	} MeshPassProcessorRenderState;

	class MeshPassProcessor
	{
	public:
		MeshPassProcessor() = default;
		~MeshPassProcessor() = default;

		void Init();
		void Process(const std::vector<MeshBatch>& batches);
		void BuildMeshDrawCommands(
			const MeshBatch& batch,
			const MeshPassProcessorRenderState& renderState
			// 0818
			);

	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) = 0; 

		friend class MeshPass;

	protected:
		std::vector<MeshDrawCommand> m_oneFrameMeshDrawCommands;
	};
}