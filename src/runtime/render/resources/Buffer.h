#pragma once

#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIResource.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>
#include <glm/glm.hpp>

namespace shzk
{
	class RHIBuffer;
	class Primitive;

	class VertexBuffer
	{
	public:
		VertexBuffer() = delete;
		VertexBuffer(const std::shared_ptr<Primitive>& primitive);	// construct interleaved buffer from primitive
		~VertexBuffer() = default;

		inline uint32_t GetVertexNum() const { return m_vertexNum; }
		inline uint32_t GetStride() const { return m_stride; }
		inline std::shared_ptr<RHIBuffer> GetBuffer() const { return m_buffer; }
	
	private:
		std::shared_ptr<RHIBuffer> m_buffer;
		uint32_t m_vertexNum	= 0;
		uint32_t m_stride		= 0;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = delete;
		IndexBuffer(const std::vector<uint32_t>& indices);
		~IndexBuffer() = default;

		inline const uint32_t GetIndexNum() const { return m_indexNum; }
		inline std::shared_ptr<RHIBuffer> GetBuffer() const{ return m_buffer; }

	private:
		std::shared_ptr<RHIBuffer> m_buffer;
		uint32_t m_indexNum = 0;
	};

	// Uniform Buffer
	template<typename T>
	class Buffer
	{
	public:
		Buffer(MemoryUsage usage = MemoryUsage::CPUToGPU, ResourceType resourceType = RESOURCE_TYPE_RW_BUFFER | RESOURCE_TYPE_UNIFORM_BUFFER)
		{
			RHIBufferInfo info{};
			info.size			= sizeof(T);
			info.memoryUsage	= usage;
			info.type			= resourceType;
			info.creationFlag	= BUFFER_CREATION_PERSISTENT_MAP;
			m_buffer = RHI::Get()->CreateBuffer(info);
		}

		std::shared_ptr<RHIBuffer> GetBuffer() const { return m_buffer; }

		void SetData(const T& data)
		{
			memcpy(m_buffer->Map(), &data, sizeof(T));
		}

		void GetData(T* data)
		{
			memcpy(data, m_buffer->Map(), sizeof(T));
		}

	private:
		std::shared_ptr<RHIBuffer> m_buffer;
	};
}