#include "MeshComponent.h"
#include "TransformComponent.h"
#include "ecs/Entity.h"

void MeshComponent::Render()
{
    if (!mesh || !material) return;

    // Get transform component
    TransformComponent* transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return;

    // Render mesh with material and transform
    // material->Bind();
    // material->SetUniform("modelMatrix", transform->GetTransformMatrix());
    // mesh->Render();
}