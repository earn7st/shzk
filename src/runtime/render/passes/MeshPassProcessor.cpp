#include "MeshPassProcessor.h"
#include "runtime/asset/Material.h"
#include "runtime/render/MeshDrawCommand.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/RenderResourceManager.h"
#include "runtime/rhi/RHICommandList.h"

#include <iostream>

namespace shzk
{
	static uint32_t GetNumPrimitives(PrimitiveType type, uint32_t indexCount)
	{
		switch (type)
		{
		case PrimitiveType::TriangleList:  return indexCount / 3;
		case PrimitiveType::TriangleStrip: return indexCount - 2;
		case PrimitiveType::LineList:      return indexCount / 2;
		case PrimitiveType::LineStrip:     return indexCount - 1;
		case PrimitiveType::PointList:     return indexCount;
		default:                           return 0;
		}
	}

	void MeshPassProcessor::Process(const std::vector<MeshBatch>& batches)
	{
		// 1. Clear
		m_oneFrameMeshDrawCommands.clear();

		// 2. Add Mesh Batches and Build Draw Commands
		for (const MeshBatch& batch : batches)
		{
			AddMeshBatch(batch);
		}

		// TODO
		// 3. Sort
 	}

	void MeshPassProcessor::BuildMeshDrawCommands(
		const MeshBatch& batch, 
		std::shared_ptr<const Material> material,
		const MeshPassProcessorRenderState& renderState, 
		std::shared_ptr<RHIShader> vertexShader, 
		std::shared_ptr<RHIShader> fragmentShader,
		RasterizerCullMode cullMode, 
		RasterizerFillMode fillMode)
	{

		const VertexFactory::VertexStreamList& streams = batch.vertexFactory->GetVertexStreams();
		std::shared_ptr<RHIDescriptorSet> materialDescriptorSet = material->GetDescriptorSet();
		for (auto& element : batch.elements)
		{
			MeshDrawCommand command{};

			MeshPushConstantData pushConstants{
				element.modelMatrix,
			};
			command.m_shaderBindings.pushConstants = pushConstants;
			command.m_shaderBindings.materialSet = materialDescriptorSet;
			
			command.m_streams = streams;
			command.m_indexBuffer = element.indexBuffer->GetBuffer();

			GraphicsMinimalPipelineState miniPSO{};
			// BoundShaderStateInput
			miniPSO.boundShaderStateInput.declaration = batch.vertexFactory->GetVertexDeclaration();
			miniPSO.boundShaderStateInput.vertexShader = vertexShader;
			miniPSO.boundShaderStateInput.fragmentShader = fragmentShader;
			// RHIBlendState
			miniPSO.blendState = renderState.blendState;
			// RHIRasterizerState
			miniPSO.rasterizerState.cullMode = cullMode;
			miniPSO.rasterizerState.fillMode = fillMode;
			// RHIDepthStencilState
			miniPSO.depthStencilState = renderState.depthStencilState;
			// PrimitiveType
			miniPSO.primitiveType = batch.primitiveType;
			command.m_state = miniPSO;

			command.m_firstIndex = element.firstIndex;
			command.m_indexCount = element.indexCount;
			// command.m_numPrimitives = GetNumPrimitives(batch.primitiveType, element.indexCount);

			m_oneFrameMeshDrawCommands.emplace_back(std::move(command));
		}
	}

	void MeshPassProcessor::ExecuteDrawCommands(std::shared_ptr<RHICommandList> cmd)
	{
		for (auto& meshDrawCommand : m_oneFrameMeshDrawCommands)
		{
			RHIGraphicsPipelineInfo pipelineInfo = BuildRHIGraphicsPipelineInfo(meshDrawCommand.m_state);
			std::shared_ptr<RHIGraphicsPipeline> pipeline = RenderResourceManager::GetGraphicsPipelineCache()->GetOrCreateGraphicsPipeline(pipelineInfo);

			meshDrawCommand.SubmitDraw(cmd, pipeline);
		}
	}

}