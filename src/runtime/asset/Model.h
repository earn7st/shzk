#pragma once
#include "runtime/asset/Asset.h"
#include "runtime/core/Transform.h"

#include <vector>
#include <memory>
#include <glm/glm.hpp>

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

	class Model : public Asset
	{
	public:
		Model() : Asset(AssetType::Model) {};	// loader to fill
		~Model() = default;

		inline const std::vector<Submesh>& GetSubmeshes() const { return m_submeshes; }

		void AddSubmesh(Submesh submesh) { m_submeshes.push_back(submesh); }

	private:
		std::string m_path;
		std::vector<Submesh> m_submeshes;
	};
}