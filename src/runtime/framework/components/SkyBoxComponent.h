#pragma once

#include "Component.h"
#include "Drawable.h"

namespace shzk
{
	class Model;
	class Material;
	class Texture;

	class SkyBoxComponent : public Component, Drawable
	{
	public:
		SkyBoxComponent();
		~SkyBoxComponent() = default;

		// virtual void CollectMeshBatch(std::vector<MeshBatch>& batches) override final;
		virtual void CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat) override final;

		void SetModel(const std::shared_ptr<Model>& model) { m_cubeModel = model; }
		void SetMaterial(const std::shared_ptr<Material>& material) { m_material = material; }

		std::shared_ptr<Model> GetModel() const { return m_cubeModel; }
		std::shared_ptr<Material> GetMaterial() const { return m_material; }

	private:
		std::shared_ptr<Model>		m_cubeModel;
		std::shared_ptr<Material>	m_material;
	};
}