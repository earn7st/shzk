#pragma once

#include <vector>

namespace shzk
{
	struct MeshBatch;

	class MeshPassProcessor
	{
	public:
		MeshPassProcessor() = default;
		~MeshPassProcessor() = default;

		virtual void BuildMeshDrawCommand();

	protected:
		virtual void Init();
		virtual void AddMeshBatch() = 0;

		friend class MeshPass;
	};
}