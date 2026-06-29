#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>

glm::mat4 Camera::GetViewMatrix() const
{
	glm::mat3 rotationMat = glm::mat3_cast(rotation);
	glm::vec3 forward = rotationMat * glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = rotationMat * glm::vec3(0.0f, 1.0f, 0.0f);
	return glm::lookAt(position, position + forward, up);
}

glm::mat3 Camera::GetProjectionMatrix(float aspect) const
{
	return glm::perspective(fovY, aspect, nearPlane, farPlane);
}
