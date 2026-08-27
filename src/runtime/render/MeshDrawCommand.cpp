#include "MeshDrawCommand.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/VertexFactory.h"
#include "runtime/rhi/RHICommandList.h"
#include "runtime/rhi/RHIResource.h"

namespace shzk
{
	bool MeshDrawCommand::SubmitDraw(std::shared_ptr<RHICommandList> cmdList)
	{
		if (SubmitDrawBegin(cmdList))
		{
			SubmitDrawEnd(cmdList);
			return true;
		}
		return false;
	}

	bool MeshDrawCommand::SubmitDrawBegin(std::shared_ptr<RHICommandList> cmdList)
	{
        if (m_shaderBindings.materialSet)
        {
            cmdList->BindDescriptorSet(m_shaderBindings.materialSet, DESCRIPTORSET_INDEX_MATERIAL);
        }

        for (uint32_t i = 0; i < m_streams.size(); ++i)
        {
            const VertexFactory::VertexStream& stream = m_streams[i];
            if (!stream.vertexBuffer) continue;

            cmdList->BindVertexBuffer(
                stream.vertexBuffer->GetBuffer(),
                i,
                stream.offset);
        }

        if (m_indexBuffer)
        {
            cmdList->BindIndexBuffer(m_indexBuffer, 0);
        }

        cmdList->PushConstants(
            &m_shaderBindings.pushConstants,
            (uint16_t)sizeof(MeshPushConstantData),
            SHADER_FREQUENCY_VERTEX);

        return true;
	}

	void MeshDrawCommand::SubmitDrawEnd(std::shared_ptr<RHICommandList> cmdList)
	{
         cmdList->DrawIndexed(m_indexCount, 1, m_firstIndex, 0, 0);
	}
}