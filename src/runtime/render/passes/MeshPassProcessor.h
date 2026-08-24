#pragma once

#include "runtime/render/MeshBatch.h"
#include "runtime/render/MeshDrawCommand.h"
#include "runtime/render/resources/VertexFactory.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vector>

namespace shzk
{
	class Shader;

	typedef struct MeshPassProcessorRenderState
	{
		RHIBlendState		blendState{};
		RHIDepthStencilState depthStencilState{};
		// uint32_t             stencilRef = 0;
	} MeshPassProcessorRenderState;

	class MeshPassProcessor
	{
	public:
		MeshPassProcessor() = default;
		~MeshPassProcessor() = default;

		void Process(const std::vector<MeshBatch>& batches);
		void BuildMeshDrawCommands(
			const MeshBatch& batch,
			// PrimitiveSceneProxy
			// MaterialRenderProxy 
			std::shared_ptr<const Material> material,
			const MeshPassProcessorRenderState& renderState,
			std::shared_ptr<RHIShader> vertexShader,
			std::shared_ptr<RHIShader> fragmentShader,
			RasterizerCullMode cullMode,	// RasterizerCullMode & RasterizerFillMode could be overrided by pass
			RasterizerFillMode fillMode);	// currently is a copy from material	
		void ExecuteDrawCommands(std::shared_ptr<RHICommandList> cmd);

		MeshPassProcessorRenderState GetRenderState() const { return m_renderState; }

	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) = 0;
		virtual RHIGraphicsPipelineInfo BuildRHIGraphicsPipelineInfo(const GraphicsMinimalPipelineState& minimal) = 0;

		friend class MeshPass;

	protected:
		MeshPassProcessorRenderState m_renderState;
		std::vector<MeshDrawCommand> m_oneFrameMeshDrawCommands;
		
	};
}