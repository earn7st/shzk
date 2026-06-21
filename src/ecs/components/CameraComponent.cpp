#include "CameraComponent.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include "TransformComponent.h"
#include "ecs/Entity.h"

glm::mat4 CameraComponent::GetViewMatrix() const
{
    // Get transform component
    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (transform) {
        // Calculate view matrix from transform
        glm::vec3 position = transform->GetPosition();
        glm::quat rotation = transform->GetRotation();

        // Forward vector (local -Z)
        glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        // Up vector (local +Y)
        glm::vec3 up = rotation * glm::vec3(0.0f, 1.0f, 0.0f);

        return glm::lookAt(position, position + forward, up);
    }
    return glm::mat4(1.0f);
}

glm::mat4 CameraComponent::GetProjectionMatrix() const
{
    if (projectionDirty) {
        projectionMatrix = glm::perspective(
            glm::radians(fieldOfView),
            aspectRatio,
            nearPlane,
            farPlane
        );
        projectionDirty = false;
    }
    return projectionMatrix;
}