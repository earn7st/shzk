#pragma once

#include "runtime/rhi/RHIDefinitions.h"

#include <array>
#include <memory>

#define MAX_VERTEX_STREAM_NUM 8

namespace shzk
{
	class VertexBuffer;

	class VertexFactory
	{
	public:
		VertexFactory() = default;
		~VertexFactory() = default;

		struct VertexStream
		{
			std::shared_ptr<const VertexBuffer> vertexBuffer = nullptr;
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
		typedef std::array<VertexStream, MAX_VERTEX_STREAM_NUM> VertexStreamList;

		virtual void InitRHIDeclaration() = 0;	// TODO: Cache

		const VertexStreamList& GetVertexStreams() const { return m_streams; }
		const std::shared_ptr<RHIVertexDeclaration>& GetVertexDeclaration() const { return m_declaration; }

		uint8_t AddVertexStream(const VertexStream& stream);

	protected:
		VertexStreamList m_streams{};
		uint8_t m_streamCount = 0;

		/** The RHI vertex declaration used to render the factory normally. */
		std::shared_ptr<RHIVertexDeclaration>	m_declaration;	

		// TODO?
		//std::shared_ptr <RHIVertexDeclaration>	m_positionDeclaration;
		//std::shared_ptr <RHIVertexDeclaration>	m_positionAndNormalDeclaration;
	};

	class InterleavedVertexFactory : public VertexFactory
	{
	public:
		InterleavedVertexFactory() = default;
		~InterleavedVertexFactory() = default;

		virtual void InitRHIDeclaration() override final;
	};
}