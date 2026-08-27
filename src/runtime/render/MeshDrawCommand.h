#pragma once

#include "runtime/render/resources/VertexFactory.h"
#include "runtime/render/resources/RenderResourceDefinitions.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <glm/glm.hpp>

namespace shzk
{
	// per primitive constant data
	typedef struct MeshPushConstantData
	{
		glm::mat4x4 modelMat;
		// TODO
		// ...
	} MeshPushConstantData;

	typedef struct MeshDrawShaderBindings
	{
		// Set 0 should be bind per-framely
		std::shared_ptr<RHIDescriptorSet>	materialSet;	// Set 1
		MeshPushConstantData				pushConstants;
	} MeshDrawShaderBindings;

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

	class RHIBuffer;
	class RHICommandList;
	class RHIGraphicsPipeline;

	class MeshDrawCommand
	{
	public:
		MeshDrawCommand() = default;
		~MeshDrawCommand() = default;

		// Resources 
		MeshDrawShaderBindings m_shaderBindings;	// TODO
		std::array<VertexFactory::VertexStream, MAX_VERTEX_STREAM_NUM> m_streams;
		std::shared_ptr<RHIBuffer>	m_indexBuffer;

		// PSO
		GraphicsMinimalPipelineState m_state;

		// Draw command parameters
		uint32_t m_firstIndex		= 0;
		uint32_t m_indexCount		= 0;
		// TODO: numInstance

	public:
		bool SubmitDraw(std::shared_ptr<RHICommandList> cmdList);

	private:
		bool SubmitDrawBegin(std::shared_ptr<RHICommandList> cmdList);
		void SubmitDrawEnd(std::shared_ptr<RHICommandList> cmdList);
	};
}