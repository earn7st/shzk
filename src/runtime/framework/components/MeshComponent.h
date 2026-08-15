#pragma once

#include "Component.h"
#include "Drawable.h"
#include "runtime/render/resources/MeshBatch.h"

namespace shzk
{
	class Model;

	class MeshComponent : public Component, Drawable
	{
	public:
		MeshComponent() : Component(ComponentType::Mesh) {}
		~MeshComponent() = default;
	
		// virtual void CollectMeshBatch(std::vector<MeshBatch>& batches) override final;
		virtual void CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat) override final;

		void SetModel(const std::shared_ptr<Model>& model) { m_model = model; }
		std::shared_ptr<Model> GetModel() const { return m_model; }

	private:
		std::shared_ptr<Model> m_model;
	};
}
