#include "FPSCameraController.h"

#include "Camera.h"

#include <SDL3/SDL.h>
#include <algorithm>

void FPSCameraController::OnMouseMove(float dx, float dy)
{
    if (bUseMouseMotion)
    {
        _accumMouseX += dx;
        _accumMouseY += dy;
	}
}

void FPSCameraController::Update(Camera& camera,
    float deltaTime,
    const bool* keyState,
    int /*numKeys*/)
{
    if (bUseMouseMotion)
    {
        _yaw -= _accumMouseX * mouseSensitivity;
        _pitch -= _accumMouseY * mouseSensitivity;
        _pitch = std::clamp(_pitch, -pitchLimit, pitchLimit);

        camera.SetYawPitch(_yaw, _pitch);
    }

    _accumMouseX = 0.0f;
    _accumMouseY = 0.0f;

    glm::vec3 moveInput(0.0f);

    if (keyState[SDL_SCANCODE_W])   moveInput.z += 1.0f;
    if (keyState[SDL_SCANCODE_S])   moveInput.z -= 1.0f;
    if (keyState[SDL_SCANCODE_A])   moveInput.x -= 1.0f;
    if (keyState[SDL_SCANCODE_D])   moveInput.x += 1.0f;
    if (keyState[SDL_SCANCODE_E])   moveInput.y += 1.0f;
    if (keyState[SDL_SCANCODE_Q])   moveInput.y -= 1.0f;

    float hLen = std::sqrt(moveInput.x * moveInput.x + moveInput.z * moveInput.z);
    if (hLen > 1.0f)
    {
        moveInput.x /= hLen;
        moveInput.z /= hLen;
    }

    glm::vec3 forward = camera.GetForward();
    glm::vec3 right = camera.GetRight();

    forward.y = 0.0f;
    float fLen = std::sqrt(forward.x * forward.x + forward.z * forward.z);
    forward = (fLen > 0.001f)
        ? glm::vec3(forward.x / fLen, 0.0f, forward.z / fLen)
        : glm::vec3(0.0f, 0.0f, -1.0f);

    right.y = 0.0f;
    float rLen = std::sqrt(right.x * right.x + right.z * right.z);
    right = (rLen > 0.001f)
        ? glm::vec3(right.x / rLen, 0.0f, right.z / rLen)
        : glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 velocity =
        forward * moveInput.z +
        right * moveInput.x +
        glm::vec3(0.0f, moveInput.y, 0.0f);

    camera.position += velocity * speed * deltaTime;
}

void FPSCameraController::ResetOrientation(float yaw, float pitch)
{
    _yaw = yaw;
    _pitch = pitch;
    _accumMouseX = 0.0f;
    _accumMouseY = 0.0f;
}