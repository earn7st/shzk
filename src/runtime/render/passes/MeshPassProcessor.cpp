#include "MeshPassProcessor.h"
#include "runtime/asset/Material.h"
#include "runtime/render/MeshDrawCommand.h"
#include "runtime/render/resources/Buffer.h"

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

		VertexFactory::VertexStreamList streams = batch.vertexFactory->GetVertexStreams();
		for (auto& element : batch.elements)
		{
			MeshDrawCommand command{};

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
			command.m_numPrimitives = GetNumPrimitives(batch.primitiveType, element.indexCount);

			m_oneFrameMeshDrawCommands.push_back(command);
		}
	}

}