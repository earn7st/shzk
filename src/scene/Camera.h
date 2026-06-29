#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Camera
{
public:
	glm::vec3 position{ 0.0f, 1.5f, 5.0f };
	glm::quat rotation{ 1.0f, 0.0f, 0.0f, 0.0f };

	float fovY		= glm::radians(60.0f);
	float nearPlane = 0.01f;
	float farPlane	= 1000.0f;

	glm::mat4 GetViewMatrix() const;
	glm::mat3 GetProjectionMatrix(float aspect) const;
};