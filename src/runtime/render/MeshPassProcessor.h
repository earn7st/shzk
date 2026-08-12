#pragma once

namespace shzk
{
	class RHIBuffer;

	class MeshDrawCommand
	{
	public:
		RHIBuffer* m_vertexBuffer;
		RHIBuffer* m_indexBuffer;
	};

	class MeshPassProcessor
	{

	};
}