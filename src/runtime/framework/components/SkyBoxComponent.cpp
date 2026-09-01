#include "SkyBoxComponent.h"

#include "runtime/asset/AssetManager.h"
#include "runtime/asset/Model.h"
#include "runtime/asset/Material.h"
#include "runtime/render/resources/Buffer.h"
#include "runtime/render/passes/RenderPass.h"

namespace shzk
{
	SkyBoxComponent::SkyBoxComponent()
		: Component(ComponentType::SkyBox)
	{
		m_cubeModel = AssetManager::GetCubeModel();
		m_material = std::make_shared<Material>();

		m_material->SetPassMask(PASS_MASK_SKY_PASS);
		m_material->SetRasterizerCullMode(RasterizerCullMode::CCW);
	}

	// SkyBox follows camera position, accumulated transform is not needed
	void SkyBoxComponent::CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat) 
	{
		if (!m_cubeModel || !m_material) return;
		auto& cubeMesh = m_cubeModel->GetSubmeshes()[0];

		std::vector<MeshBatchElement> elements{};
		MeshBatchElement element{
			.indexBuffer = cubeMesh.indexBuffer,
			.firstIndex = 0,
			.indexCount = cubeMesh.indexBuffer->GetIndexNum(),
			.modelMatrix = glm::identity<glm::mat4x4>(),
		};
		elements.push_back(element);
		batches.emplace_back(elements, cubeMesh.vertexFactory, m_material, cubeMesh.primitiveType);
	}


}