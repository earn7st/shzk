#pragma once

#include "runtime/render/resources/VertexFactory.h"
#include "runtime/rhi/RHIDefinitions.h"

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
			return	a.boundShaderStateInput == b.boundShaderStateInput &&
				a.blendState == b.blendState &&
				a.rasterizerState == b.rasterizerState &&
				a.depthStencilState == b.depthStencilState &&
				a.primitiveType == b.primitiveType;
		}
	} GraphicsMinimalPipelineState;

	class MeshDrawCommand
	{
	public:
		MeshDrawCommand() = default;
		~MeshDrawCommand() = default;

		// Resources 
		// MeshDrawShaderBindings m_shaderBindings;	// TODO
		std::array<VertexFactory::VertexStream, MAX_VERTEX_STREAM_NUM> m_streams;
		std::shared_ptr<RHIBuffer>	m_indexBuffer;

		// PSO
		GraphicsMinimalPipelineState m_state;

		// Draw command parameters
		uint32_t m_firstIndex;
		uint32_t m_numPrimitives;

	public:
		static void SubmitDraw();

	private:
		static void SubmitDrawBegin();
		static void SubmitDrawEnd();
	};
}