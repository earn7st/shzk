#pragma once

#include <memory>

namespace shzk
{
	class RHIBuffer;

	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;

		inline const uint32_t GetVertexNum() const { return m_vertexNum; }
	
	private:
		std::shared_ptr<RHIBuffer> m_interleavedBuffer;
		uint32_t m_vertexNum = 0;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() = default;

		inline const uint32_t GetIndexNum() const { return m_indexNum; }

	private:
		std::shared_ptr<RHIBuffer> m_buffer;
		uint32_t m_indexNum = 0;
	};
}