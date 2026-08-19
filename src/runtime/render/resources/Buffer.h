#pragma once

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

}