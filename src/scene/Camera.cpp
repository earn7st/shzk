#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat3 rotationMat = glm::mat3_cast(rotation);
	glm::vec3 forward = rotationMat * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = rotationMat * glm::vec3(0.0f, 1.0f, 0.0f);
	return glm::lookAtRH(position, position + forward, up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspect) const
{
	glm::mat4 proj = glm::perspective(fovY, aspect, nearPlane, farPlane);
	proj[1][1] *= -1.0f;	// Vulkan viewport
	return proj;
}

glm::vec3 Camera::GetForward() const
{
	return glm::normalize(rotation * glm::vec3(0.0f, 0.0f, -1.0f));
}

glm::vec3 Camera::GetRight() const
{
	return glm::normalize(rotation * glm::vec3(1.0f, 0.0f, 0.0f));
}
void Camera::SetYawPitch(float yaw, float pitch)
{
	glm::quat yawQuat = glm::angleAxis(yaw, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat pitchQuat = glm::angleAxis(pitch, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation = yawQuat * pitchQuat;
}

