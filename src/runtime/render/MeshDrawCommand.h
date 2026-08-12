#pragma once

#include "runtime/rhi/RHIResource.h"

namespace shzk
{
	class MeshDrawCommand
	{
	public:
		MeshDrawCommand() = delete;
		~MeshDrawCommand() = default;

	private:
		std::shared_ptr<RHIBuffer>	m_vertexBuffer;
		std::shared_ptr<RHIBuffer>	m_indexBuffer;
	};
}