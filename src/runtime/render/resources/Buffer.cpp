#include "Buffer.h"
#include "runtime/core/Primitive.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIResource.h"

namespace shzk
{
    typedef struct InterleavedVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    } InterleavedVertex;
    static_assert(sizeof(InterleavedVertex) == 32);

	VertexBuffer::VertexBuffer(const std::shared_ptr<Primitive>& primitive)
	{
        m_stride = sizeof(InterleavedVertex);
        m_vertexNum = static_cast<uint32_t>(primitive->position.size());
        if (m_vertexNum == 0) return;

        bool hasNormal      = primitive->normal.size() > 0;
        bool hasTexcoord    = primitive->texcoord.size() > 0;

        std::vector<InterleavedVertex> vertices(m_vertexNum);
        for (uint32_t i = 0; i < m_vertexNum; ++i)
        {
            vertices[i].position = primitive->position[i];
            vertices[i].normal = hasNormal
                ? primitive->normal[i] : glm::vec3(0, 0, 1);
            vertices[i].texcoord = hasTexcoord
                ? primitive->texcoord[i] : glm::vec2(0, 0);
        }

        uint32_t bufferSize = m_vertexNum * m_stride;

        RHIBufferInfo stagingInfo = {
            .size = bufferSize,
            .memoryUsage = MemoryUsage::CPUOnly,
            .type = RESOURCE_TYPE_BUFFER,
            .creationFlag = BUFFER_CREATION_PERSISTENT_MAP
        };
        auto staging = RHI::Get()->CreateBuffer(stagingInfo);
        std::memcpy(staging->Map(), vertices.data(), bufferSize);

        RHIBufferInfo gpuInfo = {
            .size = bufferSize,
            .memoryUsage = MemoryUsage::GPUOnly,
            .type = RESOURCE_TYPE_VERTEX_BUFFER,
            .creationFlag = BUFFER_CREATION_NONE
        };
        m_buffer = RHI::Get()->CreateBuffer(gpuInfo);

        auto immediateCmd = RHI::Get()->GetCommandContextImmediate();
        immediateCmd->RHICopyBuffer(staging, 0, m_buffer, 0, bufferSize);
        immediateCmd->RHISubmit();
	}

    IndexBuffer::IndexBuffer(const std::vector<uint32_t>& indices)
    {
        m_indexNum = indices.size();

        uint32_t bufferSize = m_indexNum * sizeof(uint32_t);

        RHIBufferInfo stagingInfo = {
            .size = bufferSize,
            .memoryUsage = MemoryUsage::CPUOnly,
            .type = RESOURCE_TYPE_BUFFER,
            .creationFlag = BUFFER_CREATION_PERSISTENT_MAP
        };
        auto staging = RHI::Get()->CreateBuffer(stagingInfo);
        std::memcpy(staging->Map(), indices.data(), bufferSize);

        RHIBufferInfo gpuInfo = {
            .size = bufferSize,
            .memoryUsage = MemoryUsage::GPUOnly,
            .type = RESOURCE_TYPE_INDEX_BUFFER,
            .creationFlag = BUFFER_CREATION_NONE
        };
        m_buffer = RHI::Get()->CreateBuffer(gpuInfo);

        auto immediateCmd = RHI::Get()->GetCommandContextImmediate();
        immediateCmd->RHICopyBuffer(staging, 0, m_buffer, 0, bufferSize);
        immediateCmd->RHISubmit();
    }
}