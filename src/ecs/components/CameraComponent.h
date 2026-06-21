#pragma once

#include "Component.h"

// Camera component
// Defines a viewpoint for rendering the scene by managing view and projection matrices
class CameraComponent : public Component {
private:
    float fieldOfView = 45.0f;
    float aspectRatio = 16.0f / 9.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;

    mutable glm::mat4 viewMatrix = glm::mat4(1.0f);
    mutable glm::mat4 projectionMatrix = glm::mat4(1.0f);
    mutable bool projectionDirty = true;

public:
    void SetPerspective(float fov, float aspect, float near, float far) {
        fieldOfView = fov;
        aspectRatio = aspect;
        nearPlane = near;
        farPlane = far;
        projectionDirty = true;
    }

    glm::mat4 GetViewMatrix() const;

    glm::mat4 GetProjectionMatrix() const;
};