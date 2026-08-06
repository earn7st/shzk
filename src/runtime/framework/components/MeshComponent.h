#pragma once

#include "Component.h"
#include "runtime/render/resources/Drawable.h"

namespace shzk
{
	class Model;

	class MeshComponent : public Component, Drawable
	{
	public:
		MeshComponent() : Component(ComponentType::Mesh) {}
		~MeshComponent() = default;
	
		virtual void CollectDrawBatch(std::vector<DrawBatch>& drawBatches) override final;

		std::shared_ptr<Model> GetModel() const { return m_model; }

	private:
		std::shared_ptr<Model> m_model;
	};
}
