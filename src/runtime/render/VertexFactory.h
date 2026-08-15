#pragma once

#include <array>

namespace shzk
{
	class VertexFatory
	{
	public:
		
		struct VertexStream
		{
			const VertexBuffer* vertexBuffer = nullptr;
			uint32_t offset = 0;
			uint32_t stride = 0;
			// EVertexStreamUsage VertexStreamUsage = EVertexStreamUsage::Default; // TODO
			// uint8_t	padding = 0;

			friend bool operator==(const VertexStream& a, const VertexStream& b)
			{
				return  a.vertexBuffer == b.vertexBuffer &&		// for MeshPassProcessor to merge 
					a.offset == b.offset &&
					a.stride == b.stride;
			}
		};

		typedef std::array<VertexStream, 8> VertexStreamList;

	protected:
		VertexStreamList m_streams;
	};
}