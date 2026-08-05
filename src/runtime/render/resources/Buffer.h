#pragma once

#include "runtime/rhi/RHIResource.h"

namespace shzk
{
	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		~VertexBuffer() = default;
	
	private:
		RHIBuffer m_interleavedBuffer;
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		~IndexBuffer() = default;

	private:
		RHIBuffer m_buffer;
	};
}