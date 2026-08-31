#include "VertexFactory.h"

#include "runtime/rhi/RHIDefinitions.h"

#include <cassert>

namespace shzk
{
// VertexFactory Base
	uint8_t VertexFactory::AddVertexStream(const VertexStream& stream)
	{
		for (int8_t i = 0; i < m_streamCount; ++i)
		{
			if (m_streams[i] == stream) return i;
		}
		assert(m_streamCount + 1 <= m_streams.max_size());
		m_streams[m_streamCount++] = stream;
		return m_streamCount;
	}

// Interleaved VertexFactory

	// Interleaved
	/*{
		float3 position,
		float3 normal,
		float2 texcoord,
	}*/
	void InterleavedVertexFactory::InitRHIDeclaration()
	{
		m_declaration = std::make_shared<RHIVertexDeclaration>();

		VertexElement position{};
		position.streamIndex = 0;
		position.offset = 0;
		position.type = VertexElementType::Float3;
		position.attributeIndex = 0;
		position.stride = 48;
		m_declaration->elements[0] = position;

		VertexElement normal{};
		normal.streamIndex = 0;
		normal.offset = 12;
		normal.type = VertexElementType::Float3;
		normal.attributeIndex = 1;
		normal.stride = 48;
		m_declaration->elements[1] = normal;

		VertexElement texcoord{};
		texcoord.streamIndex = 0;
		texcoord.offset = 24;
		texcoord.type = VertexElementType::Float2;
		texcoord.attributeIndex = 2;
		texcoord.stride = 48;
		m_declaration->elements[2] = texcoord;

		VertexElement tangent{};
		tangent.streamIndex = 0;
		tangent.offset = 32;
		tangent.type = VertexElementType::Float4;
		tangent.attributeIndex = 3;
		tangent.stride = 48;
		m_declaration->elements[3] = tangent;
	}
	
}