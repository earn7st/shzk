#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace shzk
{
	class RHIBuffer;
	class Primitive;

	// TODO: Check performance, padding if needed
	typedef struct InterleavedVertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texcoord;
	} InterleavedVertex;

	class VertexBuffer
	{
	public:
		VertexBuffer() = delete;
		VertexBuffer(const std::shared_ptr<Primitive>& primitive);	// construct interleaved buffer from primitive
		~VertexBuffer() = default;

		inline const uint32_t GetVertexNum() const { return m_vertexNum; }
	
	private:
		std::shared_ptr<RHIBuffer> m_interleavedBuffer;
		uint32_t m_vertexNum = 0;
		uint32_t m_interleavedStride = sizeof(InterleavedVertex);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = delete;
		IndexBuffer(const std::vector<uint32_t>& indices);
		~IndexBuffer() = default;

		inline const uint32_t GetIndexNum() const { return m_indexNum; }

	private:
		std::shared_ptr<RHIBuffer> m_buffer;
		uint32_t m_indexNum = 0;
	};
}