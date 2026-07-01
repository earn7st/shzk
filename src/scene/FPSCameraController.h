#pragma once

#include <glm/glm.hpp>

class Camera;

class FPSCameraController
{
public:
    float mouseSensitivity = 0.002f;
    float speed = 5.0f;
    float pitchLimit = 1.553f;

    FPSCameraController() = default;

    void OnMouseMove(float dx, float dy);

    void Update(Camera& camera,
        float deltaTime,
        const bool* keyState,
        int numKeys);

    void ResetOrientation(float yaw = 0.0f, float pitch = 0.0f);

	void SetUseMouseMotion(bool useMouseMotion) { bUseMouseMotion = useMouseMotion; }

private:
    bool bUseMouseMotion = false;

    float _yaw = 0.0f;
    float _pitch = 0.0f;

    float _accumMouseX = 0.0f;
    float _accumMouseY = 0.0f;
};