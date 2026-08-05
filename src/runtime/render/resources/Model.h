#pragma once

#include "runtime/core/Definitions.h"

#include <vector>
#include <memory>

namespace shzk
{
	class Primitive;
	class VertexBuffer;
	class IndexBuffer;
	class Material;

	struct Submesh
	{
		Transform localTransform;

		std::shared_ptr<Primitive>	primitive;	// cpu mesh data

		std::shared_ptr<VertexBuffer> vertexBuffer;
		std::shared_ptr<IndexBuffer> indexBuffer;
		std::shared_ptr<Material> material;
	};

	class Model
	{
	public:
		Model() = default;
		~Model() = default;

		inline const std::vector<Submesh>& GetSubmeshes() const { return m_submeshes; }

	private:
		std::vector<Submesh> m_submeshes;
	};
}